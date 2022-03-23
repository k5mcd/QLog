#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include "core/Rotator.h"
#include "RotatorWidget.h"
#include "ui_RotatorWidget.h"
#include "core/debug.h"
#include "core/Gridsquare.h"
#include "data/StationProfile.h"

MODULE_IDENTIFICATION("qlog.ui.rotatorwidget");

#define MAP_RESOLUTION 1000

RotatorWidget::RotatorWidget(QWidget *parent) :
    QWidget(parent),
    compassScene(nullptr),
    ui(new Ui::RotatorWidget)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    azimuth = 0;

    redrawMap();

    connect(Rotator::instance(), &Rotator::positionChanged, this, &RotatorWidget::positionChanged);
}

void RotatorWidget::gotoPosition() {
    FCT_IDENTIFICATION;

    int azimuth = ui->gotoSpinBox->value();
    int elevation = 0;
    Rotator::instance()->setPosition(azimuth, elevation);
}


void RotatorWidget::positionChanged(int in_azimuth, int in_elevation) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<in_azimuth<<" "<<in_elevation;
    azimuth = in_elevation;
    compassNeedle->setRotation(in_elevation);
}

void RotatorWidget::showEvent(QShowEvent* event) {
    FCT_IDENTIFICATION;

    ui->compassView->fitInView(compassScene->sceneRect(), Qt::KeepAspectRatio);
    QWidget::showEvent(event);
}

void RotatorWidget::resizeEvent(QResizeEvent* event) {
    FCT_IDENTIFICATION;

    ui->compassView->fitInView(compassScene->sceneRect(), Qt::KeepAspectRatio);
    QWidget::resizeEvent(event);
}

void RotatorWidget::redrawMap()
{
    FCT_IDENTIFICATION;

    if ( compassScene )
    {
        compassScene->deleteLater();
    }
    compassScene = new QGraphicsScene(this);
    ui->compassView->setScene(compassScene);
    ui->compassView->setStyleSheet("background-color: transparent;");

    compassScene->setSceneRect(-100, -100, 200, 200);

    QImage source(":/res/map/nasabluemarble.jpg");

    QImage map(MAP_RESOLUTION, MAP_RESOLUTION, QImage::Format_ARGB32);

    Gridsquare myGrid(StationProfilesManager::instance()->getCurProfile1().locator);

    double lat = myGrid.getLatitude();
    double lon = myGrid.getLongitude();

    double lambda0 = (lon / 180.0) * (2.0 * M_PI);
    double phi1 = - (lat / 90.0) * (0.5 * M_PI);

    for (int x = 0; x < map.width(); x++) {
        double x2 = 2.0 * M_PI * (static_cast<double>(x) / static_cast<double>(map.width()) - 0.5);
        for (int y = 0; y < map.height(); y++) {
            double y2 = 2.0 * M_PI * (static_cast<double>(y) / static_cast<double>(map.height()) - 0.5);
            double c = sqrt(x2*x2 + y2*y2);
            double phi = asin(cos(c) * sin(phi1) + y2 * sin(c) * cos(phi1) / c);

            if (c < M_PI) {
                double lambda = lambda0 + atan2(x2*sin(c), c*cos(phi1)*cos(c) - y2*sin(phi1)*sin(c));

                double s = (lambda/(2*M_PI)) + 0.5;
                double t = (phi/M_PI) + 0.5;

                int x3 = static_cast<int>(s * static_cast<double>(source.width())) % source.width();
                x3 = x3 < 0 ? x3 + source.width() : x3;

                int y3 = static_cast<int>(t * static_cast<double>(source.height())) % source.height();
                y3 = y3 < 0 ? y3 + source.height() : y3;

                map.setPixelColor(x, y, source.pixelColor(x3, y3));
            }
            else {
                map.setPixelColor(x, y, QColor(0, 0, 0, 0));
            }
        }
    }

    QGraphicsPixmapItem *pixMapItem = compassScene->addPixmap(QPixmap::fromImage(map));
    pixMapItem->moveBy(-MAP_RESOLUTION/2, -MAP_RESOLUTION/2);
    pixMapItem->setTransformOriginPoint(MAP_RESOLUTION/2, MAP_RESOLUTION/2);
    pixMapItem->setScale(200.0/MAP_RESOLUTION);

    compassScene->addEllipse(-100, -100, 200, 200, QPen(QColor(100, 100, 100), 2),
                                             QBrush(QColor(0, 0, 0), Qt::NoBrush));

    compassScene->addEllipse(-1, -1, 2, 2, QPen(Qt::NoPen),
                                             QBrush(QColor(0, 0, 0), Qt::SolidPattern));

    QPainterPath path;
    path.lineTo(-1, 0);
    path.lineTo(0, -70);
    path.lineTo(1, 0);
    path.closeSubpath();
    compassNeedle = compassScene->addPath(path, QPen(Qt::NoPen),
                    QBrush(QColor(255, 255, 255), Qt::SolidPattern));
    compassNeedle->setRotation(azimuth);
}

RotatorWidget::~RotatorWidget()
{
    delete ui;
}
