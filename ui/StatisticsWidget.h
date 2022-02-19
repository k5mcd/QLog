#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QSqlQuery>
#include <QPieSeries>
#include <QWebEngineView>

namespace Ui {
class StatisticsWidget;
}

using namespace QtCharts;

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

private:
    Ui::StatisticsWidget *ui;
    QWebEnginePage *main_page;
    bool isMainPageLoaded;
    QString postponedScripts;
};

#endif // STATISTICSWIDGET_H
