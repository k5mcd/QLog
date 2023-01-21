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
#include "core/Conditions.h"

MODULE_IDENTIFICATION("qlog.ui.onlinemapwidget");

OnlineMapWidget::OnlineMapWidget(QWidget *parent):
  QWebEngineView(parent),
  main_page(new QWebEnginePage(this)),
  isMainPageLoaded(false)
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

    QString targetJavaScript;

    if ( lat == 0 && lon == 0 )
    {
        /* just clean the last path */
        targetJavaScript = QString("if ( typeof firstpolyline !== 'undefined' ) { map.removeLayer(firstpolyline)};");
    }
    else
    {
        /* Draw a new path */
        Gridsquare myGrid(StationProfilesManager::instance()->getCurProfile1().locator);

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
                            " color: 'Fuchsia', weight: 3, opacity: 0.7, smoothFactor: 1 }); "
                            "  firstpolyline.addTo(map);"
                            " var bounds = firstpolyline.getBounds(); "
                            "map.fitBounds(bounds); "
                            "var center = bounds.getCenter(); "
                            "map.panTo(center);");
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

    if ( Conditions::instance()->isAuroraMapValid() )
    {
        const QList<AuroraMap::AuroraPoint> points = Conditions::instance()->getAuroraPoints();

        for (const AuroraMap::AuroraPoint &point : points )
        {
            if ( point.propability > 10 )
            {
                mapPoints << QString("{lat: %1, lng: %2, count: %3}").arg(point.latitude)
                                                                     .arg(point.longitude)
                                                                     .arg(point.propability);
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
    return result.join(",");
}

void OnlineMapWidget::finishLoading(bool)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    isMainPageLoaded = true;
    postponedScripts += prepareRestoreLayerStateJS();
    main_page->runJavaScript(postponedScripts);

    QFile file(":/qtwebchannel/qwebchannel.js");

    if (!file.open(QIODevice::ReadOnly))
    {
        qCInfo(runtime) << "Cannot read qwebchannel.js";
    }

    QTextStream stream(&file);
    QString js;

    js.append(stream.readAll());
    js += " var webChannel = new QWebChannel(qt.webChannelTransport, function(channel) "
          "{ window.foo = channel.objects.layerControlHandler; });"
          " map.on('overlayadd', function(e){ "
          "  switch (e.name) "
          "  { "
          "     case 'Grid': "
          "        foo.handleLayerSelectionChanged('maidenheadConfWorked', 'on'); "
          "        break; "
          "     case 'Gray-Line': "
          "        foo.handleLayerSelectionChanged('grayline', 'on'); "
          "        break; "
          "     case 'Aurora': "
          "        foo.handleLayerSelectionChanged('auroraLayer', 'on'); "
          "        break; "
          "  } "
          "});"
          "map.on('overlayremove', function(e){ "
          "   switch (e.name) "
          "   { "
          "      case 'Grid': "
          "         foo.handleLayerSelectionChanged('maidenheadConfWorked', 'off'); "
          "         break; "
          "      case 'Gray-Line': "
          "         foo.handleLayerSelectionChanged('grayline', 'off'); "
          "         break; "
          "     case 'Aurora': "
          "        foo.handleLayerSelectionChanged('auroraLayer', 'off'); "
          "        break; "
          "   } "
          "});";
    main_page->runJavaScript(js);

    auroraDataUpdate();
}

QString OnlineMapWidget::prepareRestoreLayerStateJS()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    QString js;

    settings.beginGroup("onlinemap/layoutstate");
    QStringList keys = settings.allKeys();

    for ( const QString &key : qAsConst(keys))
    {
        qCDebug(runtime) << "key:" << key << "value:" << settings.value(key);

        if ( settings.value(key).toBool() )
        {
            js += QString("map.addLayer(%1);").arg(key);
        }
        else
        {
            js += QString("map.removeLayer(%1);").arg(key);
        }
    }
    qCDebug(runtime) << js;
    return js;
}

OnlineMapWidget::~OnlineMapWidget()
{
    FCT_IDENTIFICATION;

    main_page->deleteLater();
}

void LayerControlHandler::handleLayerSelectionChanged(const QVariant &data,
                                                      const QVariant &state)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << data << state;

    QSettings settings;

    settings.setValue("onlinemap/layoutstate/" + data.toString(),
                      (state.toString().toLower() == "on") ? true : false);
}
