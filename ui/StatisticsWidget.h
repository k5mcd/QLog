#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QSqlQuery>
#include <QPieSeries>
#include <QComboBox>
#include <QWebChannel>

#include "ui/MapLayerControlHandler.h"
#include "ui/WebEnginePage.h"

namespace Ui {
class StatisticsWidget;
}

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
using namespace QtCharts;
#endif

class StatisticsWidget : public QWidget
{
    Q_OBJECT

public slots:
    void mainStatChanged(int);
    void refreshGraph();
    void dateRangeCheckBoxChanged(int);
    void mapLoaded(bool);
    void changeTheme(int);


public:
    explicit StatisticsWidget(QWidget *parent = nullptr);
    ~StatisticsWidget();

private:
    void drawBarGraphs(const QString &title, QSqlQuery &query);
    void drawPieGraph(const QString &title, QPieSeries* series);
    void drawMyLocationsOnMap(QSqlQuery &);
    void drawPointsOnMap(QSqlQuery&);
    void drawFilledGridsOnMap(QSqlQuery&);
    void refreshCallCombo();
    void refreshRigCombo();
    void refreshAntCombo();
    void refreshBandCombo();
    void refreshGridCombo();
    void refreshCombo(QComboBox * combo, QString sqlQeury);

private:
    Ui::StatisticsWidget *ui;
    WebEnginePage *main_page;
    bool isMainPageLoaded;
    QString postponedScripts;
    QWebChannel channel;
    MapLayerControlHandler layerControlHandler;
};

#endif // STATISTICSWIDGET_H
