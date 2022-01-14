#include <QGraphicsTextItem>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include <QVector3D>
#include <QtMath>
#include "OnlineMapWidget.h"
#include "core/debug.h"
#include "core/Gridsquare.h"
#include "data/StationProfile.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.onlinemapwidget");

OnlineMapWidget::OnlineMapWidget(QWidget *parent):
  QWebEngineView(parent),
  main_page(new QWebEnginePage(this)),
  isMainPageLoaded(false)
{
    FCT_IDENTIFICATION;

    setPage(main_page);
    main_page->load(QUrl(QStringLiteral("qrc:/res/map/onlinemap.html")));
    connect(this, &OnlineMapWidget::loadFinished, this, &OnlineMapWidget::finishLoading);

    setFocusPolicy(Qt::ClickFocus);
    setContextMenuPolicy(Qt::NoContextMenu);
}

void OnlineMapWidget::setTarget(double lat, double lon)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << lat << " " << lon;

    if ( !isMainPageLoaded )
    {
        return;
    }

    QString targetJavaScript;

    if ( lat == 0 && lon == 0 )
    {
        /* just clean the last path */
        targetJavaScript = QString("if ( typeof firstpolyline !== 'undefined' ) { map.removeLayer(firstpolyline)};");
    }
    else
    {
        /* Draw a new path */
        Gridsquare myGrid(StationProfilesManager::instance()->getCurrent().locator);

        double my_lat=0;
        double my_lon=0;

        if ( myGrid.isValid() )
        {
            my_lat = myGrid.getLatitude();
            my_lon = myGrid.getLongitude();
        }

        QString path = computePath(my_lat,my_lon, lat, lon);

        targetJavaScript = QString("if ( typeof firstpolyline !== 'undefined' ) { map.removeLayer(firstpolyline)}; var pointList = [ " + path + " ]; "
                            " var firstpolyline = new L.Polyline(pointList, { "
                            " color: 'red', weight: 3, opacity: 0.5, smoothFactor: 1 }); "
                            "  firstpolyline.addTo(map);"
                            " var bounds = firstpolyline.getBounds(); "
                            "map.fitBounds(bounds); "
                            "var center = bounds.getCenter(); "
                            "map.panTo(center);");
    }
    main_page->runJavaScript(targetJavaScript);
}

QString OnlineMapWidget::computePath(double lat1, double lon1, double lat2, double lon2)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << lat1 << " " << lon1 << " " << lat2 << " " << lon2;

    QStringList result;

    double latA = lat1  * M_PI / 180;
    double latB = lat2  * M_PI / 180;
    double lonA = lon1  * M_PI / 180;
    double lonB = lon2  * M_PI / 180;

    double d = 2*asin(sqrt(pow(sin(latA-latB)/2, 2) + cos(latA)* cos(latB) * pow(sin((lonA-lonB)/2), 2)));

    for (double f = 0; f <= 1; f += 0.001) {
        double A = sin((1-f)*d)/sin(d);
        double B = sin(f*d)/sin(d);
        double x = A*cos(latA)*cos(lonA) + B*cos(latB)*cos(lonB);
        double y = A*cos(latA)*sin(lonA) + B*cos(latB)*sin(lonB);
        double z = A*sin(latA)           + B*sin(latB);
        double lat = atan2(z, sqrt(x*x + y*y));
        double lon = atan2(y, x);
        result.append(QString("[%1, %2]").arg(lat*(180/M_PI)).arg(lon*(180/M_PI)));
    }
    return result.join(",");
}

void OnlineMapWidget::finishLoading(bool)
{
    FCT_IDENTIFICATION;

    isMainPageLoaded = true;
}

OnlineMapWidget::~OnlineMapWidget()
{
    FCT_IDENTIFICATION;

    main_page->deleteLater();
}
