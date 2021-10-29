#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QSqlQuery>
#include <QPieSeries>

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

public:
    explicit StatisticsWidget(QWidget *parent = nullptr);
    ~StatisticsWidget();

private:
    void drawBarGraphs(QString title, QSqlQuery query);
    void drawPieGraph(QString title, QPieSeries* series);

private:
    Ui::StatisticsWidget *ui;
};

#endif // STATISTICSWIDGET_H
