#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QMutableMapIterator>

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
    ui(new Ui::BandmapWidget)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    ui->setupUi(this);

    double freq = settings.value("newcontact/frequency", 3.5).toDouble();

    band = Data::band(freq);
    zoom = ZOOM_1KHZ;

    bandmapScene = new QGraphicsScene(this);
    connect(bandmapScene, &QGraphicsScene::focusItemChanged, this, &BandmapWidget::spotClicked);

    ui->graphicsView->setScene(bandmapScene);
    ui->graphicsView->setStyleSheet("background-color: transparent;");

    ui->clearSpotOlderSpin->setValue(settings.value("bandmap/spot_aging", 0).toInt());

    Rig* rig = Rig::instance();
    connect(rig, &Rig::frequencyChanged, this, &BandmapWidget::updateRxFrequency);

    update_timer = new QTimer;
    connect(update_timer, SIGNAL(timeout()), this, SLOT(update()));
    update_timer->start(BANDMAP_AGING_TIME);

    updateRxFrequency(freq);
    update();
}

void BandmapWidget::update() {
    FCT_IDENTIFICATION;

    bandmapScene->clear();
    bandmapAging();

    update_timer->setInterval(BANDMAP_AGING_TIME);

    // Draw Scale
    double step;
    int digits;
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
    if ( band.start >= 28.0 and band.start < 420.0 )
    {
        step = step * 10;
    }
    if ( ( band.start >= 420.0 and band.start < 2300.0 )
         || band.start == 119980 )
    {
        step = step * 100;
    }
    else if ( band.start >= 2300.0 and band.start < 75500.0 )
    {
        step = step * 1000;
    }
    else if (band.start == 75500.0 || band.start >= 142000.0)
    {
        step = step * 10000;
    }

    int steps = static_cast<int>(round((band.end - band.start) / step));

    ui->graphicsView->setFixedSize(330, steps*10 + 30);

    for (int i = 0; i <= steps; i++) {
        bandmapScene->addLine(0, i*10, (i % 5 == 0) ? 15 : 10, i*10, QPen(QColor(192,192,192)));

        if (i % 5 == 0) {
            QGraphicsTextItem* text = bandmapScene->addText(QString::number(band.start + step*i, 'f', digits));
            text->setPos(- (text->boundingRect().width()) - 10, i*10 - (text->boundingRect().height() / 2));
        }
    }

    QString endFreqDigits= QString::number(band.end + step*steps, 'f', digits);
    bandmapScene->setSceneRect(160 - (endFreqDigits.size()*10), 0, 0, steps*10 + 20);

    double y = ((rx_freq - band.start) / step) * 10;
    QPolygonF poly;
    poly << QPointF(-1, y) << QPointF(-7, y-7) << QPointF(-7, y+7);
    bandmapScene->addPolygon(poly, QPen(Qt::NoPen), QBrush(QColor(30, 180, 30), Qt::SolidPattern));

    QMap<double, DxSpot>::const_iterator lower = spots.lowerBound(band.start);
    QMap<double, DxSpot>::const_iterator upper = spots.upperBound(band.end);

    double min_y = 0;

//    {

//        double freq_y = ((14.070 - band.start) / step) * 10;
//        double text_y = std::max(min_y, freq_y);
//        bandmapScene->addLine(17, freq_y, 100, text_y);

//        QGraphicsTextItem* text = bandmapScene->addText(QString("OK1MLGtest/P/M") + "  [23:59]");
//        text->setPos(100, text_y - (text->boundingRect().height() / 2));

//    }
    QLocale locale;

    for (; lower != upper; lower++)
    {
        double freq_y = ((lower.key() - band.start) / step) * 10;
        double text_y = std::max(min_y, freq_y);
        bandmapScene->addLine(17, freq_y, 40, text_y,QPen(QColor(192,192,192)));

        QGraphicsTextItem* text = bandmapScene->addText(lower.value().callsign + " [" + lower.value().time.toString(locale.timeFormat(QLocale::ShortFormat))+"]");
        text->setPos(40, text_y - (text->boundingRect().height() / 2));
        text->setFlags(QGraphicsItem::ItemIsFocusable |
                       QGraphicsItem::ItemIsSelectable |
                       text->flags());
        text->setProperty("freq", lower.key());

        min_y = text_y + text->boundingRect().height() / 2;

        QColor textColor = Data::statusToColor(lower.value().status, qApp->palette().color(QPalette::Text));
        text->setDefaultTextColor(textColor);
    }
}

void BandmapWidget::bandmapAging()
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
    /*
    for (; lower != upper;) {
        if ( lower.value().callsign.compare(spot.callsign, Qt::CaseInsensitive) == 0 )
        {
            spots.erase(lower++);
        }
        else
        {
            ++lower;
        }
    }*/
}

void BandmapWidget::addSpot(DxSpot spot) {
    FCT_IDENTIFICATION;

    this->removeDuplicates(spot);
    spots.insert(spot.freq, spot);
    update();
}

void BandmapWidget::spotAgingChanged(int)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue("bandmap/spot_aging", ui->clearSpotOlderSpin->value());
}

void BandmapWidget::clearSpots() {
    FCT_IDENTIFICATION;

    spots.clear();
    update();
}

void BandmapWidget::zoomIn() {
    FCT_IDENTIFICATION;

    if (zoom > ZOOM_100HZ) {
        zoom = static_cast<BandmapZoom>(static_cast<int>(zoom) - 1);
    }
    update();
}

void BandmapWidget::zoomOut() {
    FCT_IDENTIFICATION;

    if (zoom < ZOOM_10KHZ) {
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

void BandmapWidget::updateRxFrequency(double freq) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << freq;

    rx_freq = freq;

    if (rx_freq < band.start || rx_freq > band.end) {
        Band newBand = Data::band(rx_freq);
        if (!newBand.name.isEmpty()) {
            band = newBand;
        }
    }

    update();
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
