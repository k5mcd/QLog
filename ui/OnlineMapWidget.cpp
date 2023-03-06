#include <QGraphicsTextItem>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include <QVector3D>
#include <QtMath>
#include <QFile>
#include <QSettings>
#include "OnlineMapWidget.h"
#include "core/debug.h"
#include "core/Gridsquare.h"
#include "data/StationProfile.h"
#include "core/debug.h"
#include "core/PropConditions.h"

MODULE_IDENTIFICATION("qlog.ui.onlinemapwidget");

OnlineMapWidget::OnlineMapWidget(QWidget *parent):
  QWebEngineView(parent),
  main_page(new WebEnginePage(this)),
  isMainPageLoaded(false),
  layerControlHandler("onlinemap",parent),
  prop_cond(nullptr)
{
    FCT_IDENTIFICATION;
    main_page->setWebChannel(&channel);

    setPage(main_page);
    main_page->load(QUrl(QStringLiteral("qrc:/res/map/onlinemap.html")));
    connect(this, &OnlineMapWidget::loadFinished, this, &OnlineMapWidget::finishLoading);

    setFocusPolicy(Qt::ClickFocus);
    setContextMenuPolicy(Qt::NoContextMenu);
    channel.registerObject("layerControlHandler", &layerControlHandler);
}

void OnlineMapWidget::setTarget(double lat, double lon)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << lat << " " << lon;

    QString targetJavaScript = QString("drawPath([]);");

    if ( lat != 0 || lon != 0 )
    {
        /* Draw a new path */
        Gridsquare myGrid(StationProfilesManager::instance()->getCurProfile1().locator);

        double my_lat=0;
        double my_lon=0;

        if ( myGrid.isValid() )
        {
            my_lat = myGrid.getLatitude();
            my_lon = myGrid.getLongitude();

            QString path = computePath(my_lat,my_lon, lat, lon);
            targetJavaScript += QString("drawPath(%1);").arg(path);
        }
    }

    if ( !isMainPageLoaded )
    {
        postponedScripts.append(targetJavaScript);
    }
    else
    {
        main_page->runJavaScript(targetJavaScript);
    }
}

void OnlineMapWidget::changeTheme(int theme)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << theme;

    QString themeJavaScript;

    if ( theme == 1 ) /* dark mode */
    {
        themeJavaScript = "map.getPanes().tilePane.style.webkitFilter=\"brightness(0.6) invert(1) contrast(3) hue-rotate(200deg) saturate(0.3) brightness(0.9)\";";
    }
    else
    {
        themeJavaScript = "map.getPanes().tilePane.style.webkitFilter=\"\";";
    }

    if ( !isMainPageLoaded )
    {
        postponedScripts.append(themeJavaScript);
    }
    else
    {
        main_page->runJavaScript(themeJavaScript);
    }
}

void OnlineMapWidget::auroraDataUpdate()
{
    FCT_IDENTIFICATION;

    QString targetJavaScript;
    QStringList mapPoints;

    if ( !prop_cond )
    {
        return;
    }

    if ( prop_cond->isAuroraMapValid() )
    {
        const QList<GenericValueMap<double>::MapPoint> points = prop_cond->getAuroraPoints();

        for (const GenericValueMap<double>::MapPoint &point : points )
        {
            if ( point.value > 10 )
            {
                mapPoints << QString("{lat: %1, lng: %2, count: %3}").arg(point.latitude)
                                                                     .arg(point.longitude)
                                                                     .arg(point.value);
            }
        }
    }

    targetJavaScript = QString(" auroraLayer.setData({max: 100, data:[%1]});").arg(mapPoints.join(","));

    qCDebug(runtime) << "Aurora JS: "<< targetJavaScript;

    if ( !isMainPageLoaded )
    {
        postponedScripts.append(targetJavaScript);
    }
    else
    {
        main_page->runJavaScript(targetJavaScript);
    }
}

void OnlineMapWidget::mufDataUpdate()
{
    FCT_IDENTIFICATION;

    QString targetJavaScript = QString("drawMuf([]);");
    QStringList mapPoints;

    if ( !prop_cond )
    {
        return;
    }

    if ( prop_cond->isMufMapValid() )
    {
        const QList<GenericValueMap<double>::MapPoint> points = prop_cond->getMUFPoints();

        for (const GenericValueMap<double>::MapPoint &point : points )
        {
            mapPoints << QString("['%1', %2, %3]").arg(QString::number(point.value,'f',0))
                                                .arg(point.latitude)
                                                .arg(point.longitude);
        }
    }

    targetJavaScript = QString(" drawMuf([%1]);").arg(mapPoints.join(","));

    qCDebug(runtime) << "MUF JS: "<< targetJavaScript;

    if ( !isMainPageLoaded )
    {
        postponedScripts.append(targetJavaScript);
    }
    else
    {
        main_page->runJavaScript(targetJavaScript);
    }
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

    double f = 0;

    for (int i = 0; i < 1000; i++)
    {
        double A = sin((1-f)*d)/sin(d);
        double B = sin(f*d)/sin(d);
        double x = A*cos(latA)*cos(lonA) + B*cos(latB)*cos(lonB);
        double y = A*cos(latA)*sin(lonA) + B*cos(latB)*sin(lonB);
        double z = A*sin(latA)           + B*sin(latB);
        double lat = atan2(z, sqrt(x*x + y*y));
        double lon = atan2(y, x);
        result.append(QString("[%1, %2]").arg(lat*(180/M_PI)).arg(lon*(180/M_PI)));

        f += 0.001;
    }
    return "[" + result.join(",") + "]";
}

void OnlineMapWidget::finishLoading(bool)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    isMainPageLoaded = true;

    /* which layers will be active */
    postponedScripts += layerControlHandler.injectMapMenuJS(true, true, true, true);

    /* focus current location */
    Gridsquare myGrid(StationProfilesManager::instance()->getCurProfile1().locator);

    if ( myGrid.isValid() )
    {
        double my_lat = myGrid.getLatitude();
        double my_lon = myGrid.getLongitude();
        QString currentProfilePosition(QString("[\"\", %1, %2, yellowIcon]").arg(my_lat).arg(my_lon));
        postponedScripts += QString("flyToPoint(%1, 4);").arg(currentProfilePosition);
    }

    main_page->runJavaScript(postponedScripts);
    layerControlHandler.restoreControls(main_page);
    auroraDataUpdate();
}

OnlineMapWidget::~OnlineMapWidget()
{
    FCT_IDENTIFICATION;

    main_page->deleteLater();
}

void OnlineMapWidget::assignPropConditions(PropConditions *conditions)
{
    FCT_IDENTIFICATION;

    prop_cond = conditions;
}
