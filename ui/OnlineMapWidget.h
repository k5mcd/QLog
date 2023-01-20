#ifndef ONLINEMAPWIDGET_H
#define ONLINEMAPWIDGET_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebChannel>

namespace Ui {
class OnlineMapWidget;
}

class LayerControlHandler : public QObject
{
    Q_OBJECT

public slots:
    void handleLayerSelectionChanged(const QVariant &data,
                                     const QVariant &state);
};

class OnlineMapWidget : public QWebEngineView
{
    Q_OBJECT

public:
    explicit OnlineMapWidget(QWidget* parent = nullptr);
    ~OnlineMapWidget();

public slots:
    void setTarget(double lat, double lon);
    void changeTheme(int);

protected slots:
    void finishLoading(bool);
    QString prepareRestoreLayerStateJS();

private:

    QWebEnginePage *main_page;
    bool isMainPageLoaded;
    QString postponedScripts;
    QWebChannel channel;
    LayerControlHandler layerControlHandler;

    QString computePath(double lat1, double lon1, double lat2, double lon2);
};

#endif // ONLINEMAPWIDGET_H
