#ifndef ONLINEMAPWIDGET_H
#define ONLINEMAPWIDGET_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebChannel>
#include "ui/MapLayerControlHandler.h"

namespace Ui {
class OnlineMapWidget;
}

class OnlineMapWidget : public QWebEngineView
{
    Q_OBJECT

public:
    explicit OnlineMapWidget(QWidget* parent = nullptr);
    ~OnlineMapWidget();

public slots:
    void setTarget(double lat, double lon);
    void changeTheme(int);
    void auroraDataUpdate();

protected slots:
    void finishLoading(bool);

private:

    QWebEnginePage *main_page;
    bool isMainPageLoaded;
    QString postponedScripts;
    QWebChannel channel;
    MapLayerControlHandler layerControlHandler;

    QString computePath(double lat1, double lon1, double lat2, double lon2);
};

#endif // ONLINEMAPWIDGET_H
