#ifndef ONLINEMAPWIDGET_H
#define ONLINEMAPWIDGET_H

#include <QWidget>
#include <QWebEngineView>

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

protected slots:
    void finishLoading(bool);

private:

    QWebEnginePage *main_page;
    bool isMainPageLoaded;
    QString postponedScripts;

    QString computePath(double lat1, double lon1, double lat2, double lon2);
};

#endif // ONLINEMAPWIDGET_H
