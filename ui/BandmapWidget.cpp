#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QMutableMapIterator>
#include <QMenu>
#include <QTextDocument>
#include <QScrollBar>
#include <QGraphicsSceneMouseEvent>

#include "BandmapWidget.h"
#include "ui_BandmapWidget.h"
#include "core/Rig.h"
#include "data/Data.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.bandmapwidget");

//Aging interval in milliseconds
#define BANDMAP_AGING_TIME 20000

BandmapWidget::BandmapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BandmapWidget),
    rxMark(nullptr),
    txMark(nullptr),
    RXPositionY(0),
    keepRXCenter(true)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    ui->setupUi(this);

    double freq = settings.value("newcontact/frequency", 3.5).toDouble();
    freq += RigProfilesManager::instance()->getCurProfile1().ritOffset;

    keepRXCenter = settings.value("bandmap/centerrx", true).toBool();

    currentBand = Data::band(freq);
    zoom = ZOOM_1KHZ;

    bandmapScene = new GraphicsScene(this);
    bandmapScene->setFocusOnTouch(false);
    connect(bandmapScene, &GraphicsScene::focusItemChanged, this, &BandmapWidget::spotClicked);

    ui->graphicsView->setScene(bandmapScene);
    ui->graphicsView->setStyleSheet("background-color: transparent;");

    ui->clearSpotOlderSpin->setValue(settings.value("bandmap/spot_aging", 0).toInt());

    Rig* rig = Rig::instance();
    connect(rig, &Rig::frequencyChanged, this, &BandmapWidget::updateTunedFrequency);

    update_timer = new QTimer;
    connect(update_timer, SIGNAL(timeout()), this, SLOT(update()));
    update_timer->start(BANDMAP_AGING_TIME);

    updateTunedFrequency(VFO1, freq, freq, freq);
    update();
}

void BandmapWidget::update()
{
    FCT_IDENTIFICATION;

    /****************
     * Restart Time *
     ****************/
    update_timer->setInterval(BANDMAP_AGING_TIME);

    /*************
     * Clear All *
     *************/
    clearAllCallsignFromScene();

    clearFreqMark(&rxMark);
    clearFreqMark(&txMark);

    bandmapScene->clear();

    /*******************
     * Determine Scale *
     *******************/
    double step;
    int digits;

    determineStepDigits(step, digits);

    int steps = static_cast<int>(round((currentBand.end - currentBand.start) / step));

    ui->graphicsView->setFixedSize(330, steps*10 + 30);

    /****************/
    /* Draw bandmap */
    /****************/
    for ( int i = 0; i <= steps; i++ )
    {
        bandmapScene->addLine(0,
                              i*10,
                              (i % 5 == 0) ? 15 : 10,
                              i*10,
                              QPen(QColor(192,192,192)));

        if (i % 5 == 0)
        {
            QGraphicsTextItem* text = bandmapScene->addText(QString::number(currentBand.start + step*i, 'f', digits));
            text->setPos(- (text->boundingRect().width()) - 10,
                         i*10 - (text->boundingRect().height() / 2));
        }
    }

    QString endFreqDigits= QString::number(currentBand.end + step*steps, 'f', digits);
    bandmapScene->setSceneRect(160 - (endFreqDigits.size()*10),
                               0,
                               0,
                               steps*10 + 20);

    /************************/
    /* Draw TX and RX Marks */
    /************************/
    drawTXRXMarks(step);

    /*****************
     * Draw Stations *
     *****************/
    updateStations();

    centerPosition();
}

void BandmapWidget::spotAging()
{
    FCT_IDENTIFICATION;

    int clear_interval_sec = ui->clearSpotOlderSpin->value() * 60;

    qCDebug(function_parameters)<<clear_interval_sec;

    if ( clear_interval_sec == 0 ) return;

    QMutableMapIterator<double, DxSpot> spotIterator(spots);

    while ( spotIterator.hasNext() )
    {
        spotIterator.next();
        //clear spots automatically
        if ( spotIterator.value().time.addSecs(clear_interval_sec) <= QDateTime::currentDateTimeUtc() )
        {
            spotIterator.remove();
        }
    }
}

void BandmapWidget::updateStations()
{
    FCT_IDENTIFICATION;

    double step;
    int digits;
    double min_y = 0;

    /****************
     * Restart Time *
     ****************/
    update_timer->setInterval(BANDMAP_AGING_TIME);

    clearAllCallsignFromScene();

    spotAging();

    determineStepDigits(step, digits);

    QMap<double, DxSpot>::const_iterator lower = spots.lowerBound(currentBand.start);
    QMap<double, DxSpot>::const_iterator upper = spots.upperBound(currentBand.end);

    for (; lower != upper; lower++)
    {
        double freq_y = ((lower.key() - currentBand.start) / step) * 10;
        double text_y = std::max(min_y + 5, freq_y);

        /*************************
         * Draw Line to Callsign *
         *************************/
        lineItemList.append(bandmapScene->addLine(17,
                                                  freq_y,
                                                  40,
                                                  text_y,
                                                  QPen(QColor(192,192,192))));

        QString callsignTmp = lower.value().callsign;
        QString timeTmp = lower.value().time.toString(locale.timeFormat(QLocale::ShortFormat));

        QGraphicsTextItem* text = bandmapScene->addText(callsignTmp + " @ " + timeTmp);
        text->document()->setDocumentMargin(0);
        text->setPos(40, text_y - (text->boundingRect().height() / 2));
        text->setFlags(QGraphicsItem::ItemIsFocusable |
                       QGraphicsItem::ItemIsSelectable |
                       text->flags());
        text->setProperty("freq", lower.key());


        min_y = text_y + text->boundingRect().height() / 2;

        QColor textColor = Data::statusToColor(lower.value().status, qApp->palette().color(QPalette::Text));
        text->setDefaultTextColor(textColor);
        textItemList.append(text);
    }
}

void BandmapWidget::determineStepDigits(double &step, int &digits)
{
    FCT_IDENTIFICATION;

    switch (zoom) {
    case ZOOM_100HZ: step = 0.0001; digits = 4; break;
    case ZOOM_250HZ: step = 0.00025; digits = 4; break;
    case ZOOM_500HZ: step = 0.0005; digits = 4; break;
    case ZOOM_1KHZ: step = 0.001; digits = 3; break;
    case ZOOM_2K5HZ: step = 0.0025; digits = 3; break;
    case ZOOM_5KHZ: step = 0.005; digits = 3; break;
    case ZOOM_10KHZ: step = 0.01; digits = 2; break;
    }

    /* bands below are too wide for BandMap, therefore it is needed to short them */
    if ( currentBand.start >= 28.0 && currentBand.start < 420.0 )
    {
        step = step * 10;
    }
    if ( ( currentBand.start >= 420.0 && currentBand.start < 2300.0 )
         || currentBand.start == 119980 )
    {
        step = step * 100;
    }
    else if ( currentBand.start >= 2300.0 && currentBand.start < 75500.0 )
    {
        step = step * 1000;
    }
    else if (currentBand.start == 75500.0 || currentBand.start >= 142000.0)
    {
        step = step * 10000;
    }
}

void BandmapWidget::clearAllCallsignFromScene()
{
    FCT_IDENTIFICATION;

    QMutableListIterator<QGraphicsLineItem*> lineIterator(lineItemList);

    while ( lineIterator.hasNext() )
    {
        lineIterator.next();
        bandmapScene->removeItem(lineIterator.value());
        delete lineIterator.value();
    }

    lineItemList.clear();

    QMutableListIterator<QGraphicsTextItem*> textIterator(textItemList);

    while ( textIterator.hasNext() )
    {
        textIterator.next();
        bandmapScene->removeItem(textIterator.value());
        delete textIterator.value();
    }

    textItemList.clear();
}

void BandmapWidget::clearFreqMark(QGraphicsPolygonItem **currentPolygon)
{
    FCT_IDENTIFICATION;

    if ( *currentPolygon != nullptr )
    {
        bandmapScene->removeItem(*currentPolygon);
        delete *currentPolygon;
        *currentPolygon = nullptr;
    }
}

void BandmapWidget::drawFreqMark(const double freq,
                                 const double step,
                                 const QColor &color,
                                 int &Yposition,
                                 QGraphicsPolygonItem **currentPolygon)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << freq << step << color;

    clearFreqMark(currentPolygon);

    /* do not show the freq mark if it is outside the bandmap */
    if ( freq < currentBand.start || freq > currentBand.end )
    {
        return;
    }

    Yposition = ((freq - currentBand.start) / step) * 10;

    QPolygonF poly;
    poly << QPointF(-1, Yposition)
         << QPointF(-7, Yposition - 7)
         << QPointF(-7, Yposition + 7);

    *currentPolygon = bandmapScene->addPolygon(poly,
                                              QPen(Qt::NoPen),
                                              QBrush(color, Qt::SolidPattern));
}

void BandmapWidget::drawTXRXMarks(double step)
{
    FCT_IDENTIFICATION;

    /**************************/
    /* Draw RX frequency mark */
    /**************************/
    drawFreqMark(rx_freq, step, QColor(30, 180, 30), RXPositionY, &rxMark);

    /**************************/
    /* Draw TX frequency mark */
    /**************************/
    if ( tx_freq >= currentBand.start
         && tx_freq <= currentBand.end
         && tx_freq != rx_freq )
    {
        int i;
        drawFreqMark(tx_freq, step, QColor(255, 0, 0), i, &txMark);
    }
}

void BandmapWidget::removeDuplicates(DxSpot &spot) {
    FCT_IDENTIFICATION;

    QMap<double, DxSpot>::iterator lower = spots.lowerBound(spot.freq - 0.005);
    QMap<double, DxSpot>::iterator upper = spots.upperBound(spot.freq + 0.005);

    while ( lower != upper )
    {
        if ( lower.value().callsign.compare(spot.callsign, Qt::CaseInsensitive) == 0 )
        {
            lower = spots.erase(lower);
        }
        else
        {
            ++lower;
        }
    }
}

void BandmapWidget::addSpot(DxSpot spot) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << spot.freq << spot.callsign;

    this->removeDuplicates(spot);
    spots.insert(spot.freq, spot);

    if ( spot.band == currentBand.name )
    {
        updateStations();
    }

}

void BandmapWidget::spotAgingChanged(int)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue("bandmap/spot_aging", ui->clearSpotOlderSpin->value());
}

void BandmapWidget::clearSpots()
{
    FCT_IDENTIFICATION;

    spots.clear();
    updateStations();
}

void BandmapWidget::zoomIn()
{
    FCT_IDENTIFICATION;

    if ( zoom > ZOOM_100HZ )
    {
        zoom = static_cast<BandmapZoom>(static_cast<int>(zoom) - 1);
    }
    update();
}

void BandmapWidget::zoomOut()
{
    FCT_IDENTIFICATION;

    if ( zoom < ZOOM_10KHZ )
    {
        zoom = static_cast<BandmapZoom>(static_cast<int>(zoom) + 1);
    }
    update();
}

void BandmapWidget::spotClicked(QGraphicsItem *newFocusItem, QGraphicsItem *, Qt::FocusReason)
{
    FCT_IDENTIFICATION;

    QGraphicsTextItem *focusedSpot = dynamic_cast<QGraphicsTextItem*>(newFocusItem);

    if ( focusedSpot )
    {
        emit tuneDx(focusedSpot->toPlainText().split(" ").first(),
                    focusedSpot->property("freq").toDouble());
    }
}

void BandmapWidget::showContextMenu(QPoint point)
{
    FCT_IDENTIFICATION;

    if ( ui->graphicsView->itemAt(point) )
    {
        return;
    }

    QMenu contextMenu(this);
    QMenu bandsMenu(tr("Show Band"), &contextMenu);

    for (Band &enabledBand : Data::enabledBandsList())
    {
        QAction* action = new QAction(enabledBand.name);
        connect(action, &QAction::triggered, this, [this, enabledBand]()
        {
            this->currentBand = enabledBand;
            this->update();
        });
        bandsMenu.addAction(action);
    }

    QAction* centerRXAction = new QAction(tr("Center RX"), &contextMenu);
    centerRXAction->setCheckable(true);
    centerRXAction->setChecked(keepRXCenter);
    connect(centerRXAction, &QAction::triggered, this, &BandmapWidget::centerRXActionChecked);

    contextMenu.addMenu(&bandsMenu);
    contextMenu.addAction(centerRXAction);

    contextMenu.exec(ui->graphicsView->mapToGlobal(point));
}

void BandmapWidget::updateTunedFrequency(VFOID vfoid, double vfoFreq, double ritFreq, double xitFreq)
{
    FCT_IDENTIFICATION;

    Q_UNUSED(vfoid)

    qCDebug(function_parameters) << vfoFreq << ritFreq << xitFreq;

    /* always show the bandmap for RIT Freq */
    rx_freq = ritFreq;

    if ( rx_freq < currentBand.start || rx_freq > currentBand.end )
    {
        /* Operator switched a band */
        Band newBand = Data::band(rx_freq);
        if ( !newBand.name.isEmpty() )
        {
            currentBand = newBand;
        }
    }

    tx_freq = xitFreq;

    double step;
    int digits;

    determineStepDigits(step, digits);

    /************************/
    /* Draw TX and RX Marks */
    /************************/
    drawTXRXMarks(step);
}

BandmapWidget::~BandmapWidget()
{
    FCT_IDENTIFICATION;

    if ( update_timer )
    {
        update_timer->stop();
        update_timer->deleteLater();
    }

    delete ui;
}

void BandmapWidget::resizeEvent(QResizeEvent *event)
{
    FCT_IDENTIFICATION;

    QWidget::resizeEvent(event);

    centerPosition();
}

void BandmapWidget::centerPosition()
{
    FCT_IDENTIFICATION;

    if ( keepRXCenter )
    {
        ui->scrollArea->verticalScrollBar()->setValue(RXPositionY - (this->height()/2) + 50);
    }
}

void BandmapWidget::centerRXActionChecked(bool state)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    keepRXCenter = state;
    settings.setValue("bandmap/centerrx", keepRXCenter);

    centerPosition();
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *evt)
{
    FCT_IDENTIFICATION;

    if ( (evt->buttons() & (Qt::MiddleButton | Qt::RightButton) )
         && items(evt->scenePos()).count())
    {
        evt->accept();
    }
    else
    {
        QGraphicsScene::mousePressEvent(evt);
    }
}

void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *evt)
{
    FCT_IDENTIFICATION;

    if ( (evt->buttons() & (Qt::MiddleButton | Qt::RightButton) )
         && items(evt->scenePos()).count())
    {
        evt->accept();
    }
    else
    {
        QGraphicsScene::mouseDoubleClickEvent(evt);
    }
}
