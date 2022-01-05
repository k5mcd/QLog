#include <QChart>
#include <QChartView>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QBarSeries>
#include <QSqlQuery>
#include <QDateTime>
#include <QDebug>
#include <QComboBox>
#include "StatisticsWidget.h"
#include "ui_StatisticsWidget.h"
#include "core/debug.h"
#include "models/SqlListModel.h"
#include <core/Gridsquare.h>

MODULE_IDENTIFICATION("qlog.ui.statisticswidget");

void StatisticsWidget::mainStatChanged(int idx)
{
     FCT_IDENTIFICATION;

     qCDebug(function_parameters) << idx;

     ui->statTypeSecCombo->blockSignals(true);

     ui->statTypeSecCombo->clear();

     switch ( idx )
     {
     /* QSOs per */
     case 0:
     {
         ui->statTypeSecCombo->addItem(tr("Year"));
         ui->statTypeSecCombo->addItem(tr("Month"));
         ui->statTypeSecCombo->addItem(tr("Day in Week"));
         ui->statTypeSecCombo->addItem(tr("Hour"));
         ui->statTypeSecCombo->addItem(tr("Mode"));
         ui->statTypeSecCombo->addItem(tr("Band"));
         ui->statTypeSecCombo->addItem(tr("Continent"));
         ui->statTypeSecCombo->addItem(tr("Propagation Mode"));
     }
     break;

     /* Percents */
     case 1:
     {
         ui->statTypeSecCombo->addItem(tr("Confirmed / Not Confirmed"));
     }
     break;

     /* TOP 10 */
     case 2:
     {
         ui->statTypeSecCombo->addItem(tr("Countries"));
         ui->statTypeSecCombo->addItem(tr("Big Gridsquares"));
     }
     break;

     /* Histogram */
     case 3:
     {
         ui->statTypeSecCombo->addItem(tr("Distance"));
     }
     break;

     /* Show on Map */
     case 4:
     {
         ui->statTypeSecCombo->addItem(tr("QSOs"));
         ui->statTypeSecCombo->addItem(tr("Confirmed/Worked Grids"));
     }
     break;
     }

     if ( idx == 4 )
     {
         ui->lotwCheckBox->setEnabled(true);
         ui->eqslCheckBox->setEnabled(true);
         ui->paperCheckBox->setEnabled(true);
     }
     else
     {
         ui->lotwCheckBox->setEnabled(false);
         ui->eqslCheckBox->setEnabled(false);
         ui->paperCheckBox->setEnabled(false);
     }

     ui->statTypeSecCombo->blockSignals(false);

     refreshGraph();
}

void StatisticsWidget::refreshGraph()
{
     FCT_IDENTIFICATION;

     QStringList genericFilter;

     genericFilter << " 1 = 1 "; //just initialization - use only in case of empty Options

     if ( ui->myCallCombo->currentIndex() != 0 )
     {
         genericFilter << " (station_callsign = '" + ui->myCallCombo->currentText() + "') ";
     }

     if ( ui->myGridCombo->currentIndex() != 0 )
     {
         if ( ui->myGridCombo->currentText().isEmpty() )
         {
             genericFilter << " (my_gridsquare is NULL) ";
         }
         else
         {
             genericFilter << " (my_gridsquare = '" + ui->myGridCombo->currentText() + "') ";
         }
     }

     if ( ui->myRigCombo->currentIndex() != 0 )
     {
         if ( ui->myRigCombo->currentText().isEmpty() )
         {
             genericFilter << " (my_rig is NULL) ";
         }
         else
         {
             genericFilter << " (my_rig = '" + ui->myRigCombo->currentText() + "') ";
         }
     }

     if ( ui->myAntennaCombo->currentIndex() != 0 )
     {
         if ( ui->myAntennaCombo->currentText().isEmpty() )
         {
             genericFilter << " (my_antenna is NULL) ";
         }
         else
         {
             genericFilter << " (my_antenna = '" + ui->myAntennaCombo->currentText() + "') ";
         }
     }

     if ( ui->bandCombo->currentIndex() != 0 )
     {
         if ( ! ui->bandCombo->currentText().isEmpty() )
         {
             genericFilter << " (band = '" + ui->bandCombo->currentText() + "') ";
         }
     }

     if ( ui->useDateRangeCheckBox->isChecked() )
     {
         genericFilter << " (start_time BETWEEN '" + ui->startDateEdit->date().toString("yyyy-MM-dd")
                          + "' AND '" + ui->endDateEdit->date().toString("yyyy-MM-dd") + "' ) ";
     }

     qCDebug(runtime) << "main " << ui->statTypeMainCombo->currentIndex()
                      << " secondary " << ui->statTypeSecCombo->currentIndex();

     if ( ui->statTypeMainCombo->currentIndex() == 0 )
     {
         QString stmt;

         switch ( ui->statTypeSecCombo->currentIndex() )
         {

         case 0:
         case 1:
         case 2:
         case 3:
         {
             QString startGenerator = "1";
             QString endGenerator = "12";
             QString formatGenerator = "%m";
             QString XYMapping = "col1, SUM(cnt)";

             if ( ui->statTypeSecCombo->currentIndex() == 0 )
             {
                 if ( ui->useDateRangeCheckBox->isChecked() )
                 {
                     startGenerator = ui->startDateEdit->date().toString("yyyy");
                     endGenerator = ui->endDateEdit->date().toString("yyyy");
                 }
                 else
                 {
                    startGenerator = "CAST(MIN(start_time) as INTEGER) from contacts";
                    endGenerator = " (select strftime('%Y', DATE()))";
                 }
                 formatGenerator = "%Y";
             }
             else if ( ui->statTypeSecCombo->currentIndex() == 1 )
             {
                 startGenerator = "1";
                 endGenerator = "12";
                 formatGenerator = "%m";
             }
             else if ( ui->statTypeSecCombo->currentIndex() == 2 )
             {
                 startGenerator = "0";
                 endGenerator = "6";
                 formatGenerator = "%w";
                 XYMapping = "case col1 when 0 THEN '" + tr("Sun") + "' "
                             "WHEN 1 THEN '" + tr("Mon") + "' "
                             "WHEN 2 THEN '" + tr("Tue") + "' "
                             "WHEN 3 THEN '" + tr("Wed") + "' "
                             "WHEN 4 THEN '" + tr("Thu") + "' "
                             "WHEN 5 THEN '" + tr("Fri") + "' "
                             "ELSE '" + tr("Sat") + "' END, "
                             "SUM(cnt) ";
             }
             else if ( ui->statTypeSecCombo->currentIndex() == 3 )
             {
                 startGenerator = "0";
                 endGenerator = "23";
                 formatGenerator = "%H";
             }

             stmt = "WITH RECURSIVE cnt(incnt) AS ( "
                    " SELECT " + startGenerator + " "
                    " UNION ALL "
                    " SELECT incnt + 1 "
                    " FROM cnt "
                    " WHERE incnt < " + endGenerator + " "
                    " ) "
                    " SELECT " + XYMapping + " "
                    " FROM "
                    " ( "
                    "   SELECT  incnt as col1, 0 as cnt from cnt "
                    "   UNION ALL "
                    "   SELECT CAST(strftime('" + formatGenerator +"', start_time) as INTEGER) as col1, count(1) as cnt "
                    "   FROM contacts "
                    "   WHERE " + genericFilter.join(" AND ") + " "
                    "   GROUP BY col1 "
                    " ) "
                    " GROUP BY col1 "
                    " ORDER BY col1";
         }
             break;
         case 4:
             stmt = "SELECT mode, COUNT(1) FROM contacts WHERE "
                     + genericFilter.join(" AND ") + " GROUP BY mode ORDER BY mode";
             break;
         case 5:
             stmt = "SELECT band, cnt FROM (SELECT band, start_freq, COUNT(1) AS cnt FROM contacts c, bands b WHERE "
                    + genericFilter.join(" AND ")
                    + " AND c.band = b.name GROUP BY band, start_freq) ORDER BY start_freq";
             break;
         case 6:
             stmt = "SELECT cont, COUNT(1) FROM contacts WHERE "
                    + genericFilter.join(" AND ")
                    + " GROUP BY cont ORDER BY cont";
             break;
         case 7:
             stmt = "SELECT IFNULL(prop_mode, '" + tr("Not specified") + "'), COUNT(1) FROM contacts WHERE "
                    + genericFilter.join(" AND ") + " GROUP BY prop_mode ORDER BY prop_mode";
             break;
         }

         qCDebug(runtime) << stmt;

         QSqlQuery query(stmt);

         drawBarGraphs(ui->statTypeMainCombo->currentText()
                       + " "
                       + ui->statTypeSecCombo->currentText(),
                       query);
     }
     else if ( ui->statTypeMainCombo->currentIndex() == 1 )
     {
         QString stmt;

         switch ( ui->statTypeSecCombo->currentIndex() )
         {

         case 0:
             stmt = "SELECT (1.0 * COUNT(1)/(SELECT COUNT(1) AS total_cnt FROM contacts)) * 100 FROM contacts WHERE "
                    + genericFilter.join(" AND ")
                    + " AND eqsl_qsl_rcvd = 'Y' OR lotw_qsl_rcvd = 'Y' OR qsl_rcvd = 'Y'";
             break;
         }

         QSqlQuery query(stmt);

         qCDebug(runtime) << stmt;

         QPieSeries *series = new QPieSeries();

         query.next();

         float confirmed = query.value(0).toInt();
         float notConfirmed = 100.0 - confirmed;

         series->append(tr("Confirmed ") + QString::number(confirmed) + "%", confirmed);
         series->append(tr("Not Confirmed ") + QString::number(notConfirmed) + "%", notConfirmed);
         series->setLabelsVisible(true);

         drawPieGraph(QString(), series);
     }
     else if ( ui->statTypeMainCombo->currentIndex() == 2 )
     {
         QString stmt;

         switch ( ui->statTypeSecCombo->currentIndex() )
         {
         case 0:
             stmt = "SELECT d.name, COUNT(1) AS cnt FROM contacts c, dxcc_entities d WHERE "
                    + genericFilter.join(" AND ")
                    + " AND c.dxcc = d.id GROUP BY d.name ORDER BY cnt DESC LIMIT 10";
             break;
         case 1:
             stmt = "SELECT SUBSTR(gridsquare,1,4), COUNT(1) AS cnt FROM contacts WHERE gridsquare IS NOT NULL GROUP by SUBSTR(gridsquare,1,4) ORDER BY cnt DESC LIMIT 10";
             break;
         }

         qCDebug(runtime) << stmt;

         QSqlQuery query(stmt);

         drawBarGraphs(ui->statTypeMainCombo->currentText()
                       + " "
                       + ui->statTypeSecCombo->currentText(),
                       query);

     }
     else if ( ui->statTypeMainCombo->currentIndex() == 3 )
     {
         QString stmt;

         switch ( ui->statTypeSecCombo->currentIndex() )
         {
         case 0:
             stmt = "WITH hist AS ( "
                    " SELECT CAST(distance/500.00 AS INTEGER) * 500 as dist_floor, "
                    " COUNT(1) AS count "
                    " FROM contacts "
                    " WHERE " + genericFilter.join(" AND ") + " AND distance IS NOT NULL "
                    " GROUP BY 1 "
                    " ORDER BY 1 "
                    " ) "
                    //" SELECT dist_floor || ' - ' || (dist_floor + 500) as dist_range, count "
                    "SELECT dist_floor as dist_range, count "
                    " FROM hist "
                    " ORDER BY 1";
             break;
         }

         qCDebug(runtime) << stmt;

         QSqlQuery query(stmt);

         drawBarGraphs(ui->statTypeMainCombo->currentText()
                       + " "
                       + ui->statTypeSecCombo->currentText(),
                       query);
     }
     else if ( ui->statTypeMainCombo->currentIndex() == 4 )
     {
         QStringList confirmed("1=2 ");

         if ( ui->eqslCheckBox->isChecked() )
         {
             confirmed << " eqsl_qsl_rcvd = 'Y' ";
         }

         if ( ui->lotwCheckBox->isChecked() )
         {
             confirmed << " lotw_qsl_rcvd = 'Y' ";
         }

         if ( ui->paperCheckBox->isChecked() )
         {
             confirmed << " qsl_rcvd = 'Y' ";
         }

         QString innerCase = " CASE WHEN (" + confirmed.join("or") + ") THEN 1 ELSE 0 END ";


         QString stmtMyLocations = "SELECT DISTINCT my_gridsquare FROM contacts WHERE " + genericFilter.join(" AND ");
         QSqlQuery myLocations(stmtMyLocations);
         qCDebug(runtime) << stmtMyLocations;

         drawMyLocationsOnMap(myLocations);

         QString stmt = "SELECT callsign, gridsquare, SUM(confirmed) FROM (SELECT callsign, gridsquare, "
                        + innerCase +" AS confirmed FROM contacts WHERE gridsquare is not NULL AND "
                        + genericFilter.join(" AND ") +" ) GROUP BY callsign, gridsquare";
         QSqlQuery query(stmt);
         qCDebug(runtime) << stmt;

         switch ( ui->statTypeSecCombo->currentIndex() )
         {
         case 0:
             drawPointsOnMap(query);
             break;

         case 1:
             drawFilledGridsOnMap(query);
             break;
         }

         ui->stackedWidget->setCurrentIndex(1);
     }
}

void StatisticsWidget::dateRangeCheckBoxChanged(int)
{
    FCT_IDENTIFICATION;

    if ( ui->useDateRangeCheckBox->isChecked() )
    {
        ui->startDateEdit->setEnabled(true);
        ui->endDateEdit->setEnabled(true);
    }
    else
    {
        ui->startDateEdit->setEnabled(false);
        ui->endDateEdit->setEnabled(false);
    }

    refreshGraph();
}

void StatisticsWidget::mapLoaded(bool)
{
    FCT_IDENTIFICATION;

    isMainPageLoaded = true;
}

StatisticsWidget::StatisticsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatisticsWidget),
    main_page(new QWebEnginePage),
    isMainPageLoaded(false)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    ui->myCallCombo->setModel(new SqlListModel("SELECT DISTINCT UPPER(station_callsign) FROM contacts ORDER BY station_callsign", tr("All"), this));
    ui->myGridCombo->setModel(new SqlListModel("SELECT DISTINCT UPPER(my_gridsquare) FROM contacts ORDER BY my_gridsquare", tr("All"), this));
    ui->myRigCombo->setModel(new SqlListModel("SELECT DISTINCT my_rig FROM contacts ORDER BY my_gridsquare", tr("All"), this));
    ui->myAntennaCombo->setModel(new SqlListModel("SELECT DISTINCT my_antenna FROM contacts ORDER BY my_gridsquare", tr("All"), this));
    ui->bandCombo->setModel(new SqlListModel("SELECT name FROM bands ORDER BY start_freq", tr("All"), this));

    ui->graphView->setRenderHint(QPainter::Antialiasing);
    ui->graphView->setChart(new QChart());

    ui->mapView->setPage(main_page);
    main_page->load(QUrl(QStringLiteral("qrc:/res/map/onlinemap.html")));
    ui->mapView->setFocusPolicy(Qt::ClickFocus);
    connect(ui->mapView, &QWebEngineView::loadFinished, this, &StatisticsWidget::mapLoaded);

    mainStatChanged(0);
}

StatisticsWidget::~StatisticsWidget()
{
    FCT_IDENTIFICATION;
    main_page->deleteLater();
    delete ui;
}

void StatisticsWidget::drawBarGraphs(const QString &title, QSqlQuery &query)
{
    FCT_IDENTIFICATION;

    if ( query.lastQuery().isEmpty() ) return;

    QChart *chart = ui->graphView->chart();
    QBarSet* set = new QBarSet(title);
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    QBarSeries* series = new QBarSeries();
    QValueAxis *axisY = new QValueAxis();

    if ( chart != nullptr )
    {
        chart->deleteLater();
    }

    chart = new QChart();

    while ( query.next() )
    {
        axisX->append(query.value(0).toString());
        *set << query.value(1).toInt();
    }

    series->append(set);
    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY->setTickCount(10);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    axisY->applyNiceNumbers();
    axisY->setLabelFormat("%d");

    series->setLabelsPosition(QAbstractBarSeries::LabelsInsideEnd);
    series->setLabelsVisible(true);

    chart->setTitle(title);
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::SeriesAnimations);

    ui->stackedWidget->setCurrentIndex(0);
    ui->graphView->setChart(chart);
}

void StatisticsWidget::drawPieGraph(const QString &title, QPieSeries *series)
{
    FCT_IDENTIFICATION;

    QChart *chart = ui->graphView->chart();

    if ( chart != nullptr )
    {
        chart->deleteLater();
    }


    chart = new QChart();

    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setTitle(title);

    ui->stackedWidget->setCurrentIndex(0);
    ui->graphView->setChart(chart);
}

void StatisticsWidget::drawMyLocationsOnMap(QSqlQuery &query)
{
    FCT_IDENTIFICATION;

    if ( !isMainPageLoaded )
    {
        return;
    }

    if ( query.lastQuery().isEmpty() ) return;

    QList<QString> locations;

    while ( query.next() )
    {
        QString loc = query.value(0).toString();
        Gridsquare stationGrid(loc);

        if ( stationGrid.isValid() )
        {
            double lat = stationGrid.getLatitude();
            double lon = stationGrid.getLongitude();
            locations.append(QString("[\"%1\", %2, %3]").arg(loc).arg(lat).arg(lon));
        }
    }

    QString javaScript = QString("grids_confirmed = [];"
                                 "grids_worked = [];"
                                 "if ( typeof myLocGroup !== 'undefined' ) { map.removeLayer(myLocGroup)};"
                                 " var myLocGroup = L.layerGroup().addTo(map); "
                                 " mylocations = [ %1 ]; "
                                 " for (var i = 0; i < mylocations.length; i++) { "
                                 "   myLocGroup.addLayer(L.marker([mylocations[i][1], mylocations[i][2]],{icon: homeIcon}) "
                                 "   .bindPopup(mylocations[i][0])); }"
                                 "maidenheadConfWorked.redraw();").arg(locations.join(","));

    qCDebug(runtime) << javaScript;

    main_page->runJavaScript(javaScript);
}

void StatisticsWidget::drawPointsOnMap(QSqlQuery &query)
{
    FCT_IDENTIFICATION;

    if ( !isMainPageLoaded )
    {
        return;
    }

    if ( query.lastQuery().isEmpty() ) return;

    QList<QString> stations;

    while ( query.next() )
    {
        Gridsquare stationGrid(query.value(1).toString());

        if ( stationGrid.isValid() )
        {
            double lat = stationGrid.getLatitude();
            double lon = stationGrid.getLongitude();
            stations.append(QString("[\"%1\", %2, %3, %4]").arg(query.value(0).toString()).arg(lat).arg(lon).arg(query.value(2).toInt()));
        }
    }

    QString javaScript = QString("grids_confirmed = [];"
                                 "grids_worked = [];"
                                 "if ( typeof QSOGroup !== 'undefined' ) { map.removeLayer(QSOGroup)};"
                                 " var QSOGroup = L.layerGroup().addTo(map); "
                                 " locations = [ %1 ]; "
                                 " for (var i = 0; i < locations.length; i++) { "
                                 "   QSOGroup.addLayer(L.marker([locations[i][1], locations[i][2]],{icon: (locations[i][3] > 0) ? greenIcon: yellowIcon}) "
                                 "   .bindPopup(locations[i][0])); }"
                                 "maidenheadConfWorked.redraw();").arg(stations.join(","));

    qCDebug(runtime) << javaScript;

    main_page->runJavaScript(javaScript);
}

void StatisticsWidget::drawFilledGridsOnMap(QSqlQuery &query)
{
    FCT_IDENTIFICATION;

    if ( !isMainPageLoaded )
    {
        return;
    }

    if ( query.lastQuery().isEmpty() ) return;

    QList<QString> confirmedGrids;
    QList<QString> workedGrids;

    while ( query.next() )
    {
        if ( query.value(2).toInt() > 0 && ! confirmedGrids.contains(query.value(1).toString()) )
        {
            confirmedGrids << QString("\"" + query.value(1).toString() + "\"");
        }
        else
        {
            workedGrids << QString("\"" + query.value(1).toString() + "\"");
        }
    }

    QString javaScript = QString(" grids_confirmed = [ %1 ]; "
                                 " grids_worked = [ %2 ];"
                                 " mylocations = [];"
                                 " locations = [];"
                                 "if ( typeof QSOGroup !== 'undefined' ) { map.removeLayer(QSOGroup)};"
                                 "maidenheadConfWorked.redraw();").arg(confirmedGrids.join(","), workedGrids.join(","));

    qCDebug(runtime) << javaScript;

    main_page->runJavaScript(javaScript);

}
