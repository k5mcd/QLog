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
#include "data/Band.h"
#include "data/Data.h"

MODULE_IDENTIFICATION("qlog.ui.onlinemapwidget");

OnlineMapWidget::OnlineMapWidget(QWidget *parent):
  QWebEngineView(parent),
  main_page(new WebEnginePage(this)),
  isMainPageLoaded(false),
  webChannelHandler("onlinemap",parent),
  prop_cond(nullptr)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    main_page->setWebChannel(&channel);

    setPage(main_page);
    main_page->load(QUrl(QStringLiteral("qrc:/res/map/onlinemap.html")));
    connect(this, &OnlineMapWidget::loadFinished, this, &OnlineMapWidget::finishLoading);

    setFocusPolicy(Qt::ClickFocus);
    setContextMenuPolicy(Qt::NoContextMenu);
    channel.registerObject("layerControlHandler", &webChannelHandler);

    double freq = settings.value("newcontact/frequency", 3.5).toDouble();
    freq += RigProfilesManager::instance()->getCurProfile1().ritOffset;

    setIBPBand(VFO1, 0.0, freq, 0.0);
}

void OnlineMapWidget::setTarget(double lat, double lon)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << lat << " " << lon;

    QString targetJavaScript;

    if ( ! qIsNaN(lat) && ! qIsNaN(lon) )
    {
        /* Draw a new path */
        Gridsquare myGrid(StationProfilesManager::instance()->getCurProfile1().locator);

        if ( myGrid.isValid() )
        {
            double my_lat=0;
            double my_lon=0;
            my_lat = myGrid.getLatitude();
            my_lon = myGrid.getLongitude();

            targetJavaScript += QString("drawPath([{lat: %1, lng: %2}, {lat: %3, lng: %4}]);").arg(my_lat)
                                                                                              .arg(my_lon)
                                                                                              .arg(lat)
                                                                                              .arg(lon);
        }
    }
    else
    {
        targetJavaScript = QString("drawPath([]);");
    }

    runJavaScript(targetJavaScript);
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

    runJavaScript(themeJavaScript);
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

    runJavaScript(targetJavaScript);
}

void OnlineMapWidget::mufDataUpdate()
{
    FCT_IDENTIFICATION;

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

    QString targetJavaScript = QString(" drawMuf([%1]);").arg(mapPoints.join(","));

    runJavaScript(targetJavaScript);
}

void OnlineMapWidget::setIBPBand(VFOID , double, double ritFreq, double)
{
    FCT_IDENTIFICATION;

    Band newBand = Data::band(ritFreq);

    QString targetJavaScript = QString("currentBand=\"%1\";").arg(newBand.name);

    runJavaScript(targetJavaScript);
}

void OnlineMapWidget::finishLoading(bool)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    isMainPageLoaded = true;

    /* which layers will be active */
    postponedScripts += webChannelHandler.generateMapMenuJS(true, true, true, true, true);

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
    webChannelHandler.restoreLayerControlStates(main_page);
    auroraDataUpdate();
}

void OnlineMapWidget::runJavaScript(QString &js)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << js;

    if ( !isMainPageLoaded )
    {
        postponedScripts.append(js);
    }
    else
    {
        main_page->runJavaScript(js);
    }
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
