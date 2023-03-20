#ifndef ONLINEMAPWIDGET_H
#define ONLINEMAPWIDGET_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebChannel>
#include "ui/MapWebChannelHandler.h"
#include "core/PropConditions.h"
#include "ui/WebEnginePage.h"
#include "core/Rig.h"

namespace Ui {
class OnlineMapWidget;
}

class OnlineMapWidget : public QWebEngineView
{
    Q_OBJECT

public:
    explicit OnlineMapWidget(QWidget* parent = nullptr);
    ~OnlineMapWidget();

    void assignPropConditions(PropConditions *);

public slots:
    void setTarget(double lat, double lon);
    void changeTheme(int);
    void auroraDataUpdate();
    void mufDataUpdate();
    void setIBPBand(VFOID, double, double, double);

protected slots:
    void finishLoading(bool);

private:

    WebEnginePage *main_page;
    bool isMainPageLoaded;
    QString postponedScripts;
    QWebChannel channel;
    MapWebChannelHandler layerControlHandler;
    PropConditions *prop_cond;
    QString computePath(double lat1, double lon1, double lat2, double lon2);
};

#endif // ONLINEMAPWIDGET_H
