#include <QPushButton>
#include <QSqlQuery>
#include "AwardsDialog.h"
#include "ui_AwardsDialog.h"
#include "models/SqlListModel.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.awardsdialog");

AwardsDialog::AwardsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AwardsDialog),
    detailedViewModel(new AwardsTableModel(this))
{
    FCT_IDENTIFICATION;
    ui->setupUi(this);

    ui->myCallComboBox->setModel(new SqlListModel("SELECT DISTINCT UPPER(station_callsign) FROM contacts ORDER BY station_callsign", ""));

    ui->awardComboBox->addItem(tr("DXCC"), QVariant("dxcc"));
    ui->awardComboBox->addItem(tr("ITU"), QVariant("itu"));
    ui->awardComboBox->addItem(tr("WAC"), QVariant("wac"));
    ui->awardComboBox->addItem(tr("WAZ"), QVariant("waz"));
    ui->awardComboBox->addItem(tr("IOTA"), QVariant("iota"));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Done"));

    ui->awardTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->awardTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    refreshTable(0);
}

AwardsDialog::~AwardsDialog()
{
    FCT_IDENTIFICATION;
    delete ui;
    detailedViewModel->deleteLater();
}

void AwardsDialog::refreshTable(int idx)
{
    FCT_IDENTIFICATION;

    QString awardSelected = ui->awardComboBox->itemData(ui->awardComboBox->currentIndex()).toString();
    QStringList confirmed("1=2 ");
    QStringList modes("'NONE'");
    QString headersColumns;
    QString uniqColumns;
    QString sqlPart = "FROM contacts c, modes m  "
                      "WHERE c.mode = m.name"
                      "      AND c.station_callsign = '" + ui->myCallComboBox->currentText() + "' ";
    QString excludePart;

    if ( ui->cwCheckBox->isChecked() )
    {
        modes << "'CW'";
    }

    if ( ui->phoneCheckBox->isChecked() )
    {
        modes << "'PHONE'";
    }

    if ( ui->digiCheckBox->isChecked() )
    {
        modes << "'DIGITAL'";

    }

    if ( awardSelected == "dxcc" )
    {
        headersColumns = "d.name col1, d.prefix col2 ";
        uniqColumns = "c.dxcc";
        sqlPart = " FROM dxcc_entities d "
                  "     LEFT OUTER JOIN contacts c ON d.id = c.dxcc "
                  "     LEFT OUTER JOIN modes m on c.mode = m.name "
                  "WHERE (c.id is NULL or c.station_callsign = '" + ui->myCallComboBox->currentText() + "') ";
    }
    else if ( awardSelected == "waz" )
    {
        headersColumns = "c.cqz col1, NULL col2 ";
        uniqColumns = "c.cqz";
    }
    else if ( awardSelected == "itu" )
    {
        headersColumns = "c.ituz col1, NULL col2 ";
        uniqColumns = "c.ituz";
    }
    else if ( awardSelected == "wac" )
    {
        headersColumns = "c.cont col1, NULL col2 ";
        uniqColumns = "c.cont";
    }
    else if ( awardSelected == "iota" )
    {
        headersColumns = "c.iota col1, NULL col2 ";
        uniqColumns = "c.iota";
        excludePart = " AND c.iota is not NULL ";
    }

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

    QString innerCase = " CASE WHEN (" + confirmed.join("or") + ") THEN 2 ELSE 1 END ";

    detailedViewModel->setQuery(
                    "WITH dxcc_summary AS ( "
                    "SELECT  " + headersColumns +", "
                    "    MAX(CASE WHEN band = '160m' AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '160m', "
                    "    MAX(CASE WHEN band = '80m'  AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '80m', "
                    "    MAX(CASE WHEN band = '40m'  AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '40m', "
                    "    MAX(CASE WHEN band = '30m'  AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '30m', "
                    "    MAX(CASE WHEN band = '20m'  AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '20m', "
                    "    MAX(CASE WHEN band = '17m'  AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '17m', "
                    "    MAX(CASE WHEN band = '15m'  AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '15m', "
                    "    MAX(CASE WHEN band = '12m'  AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '12m', "
                    "    MAX(CASE WHEN band = '10m'  AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '10m', "
                    "    MAX(CASE WHEN band = '6m'   AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '6m', "
                    "    MAX(CASE WHEN band = '2m'   AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '2m', "
                    "    MAX(CASE WHEN band = '70cm' AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as '70cm', "
                    "    MAX(CASE WHEN prop_mode = 'SAT' AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as 'SAT', "
                    "    MAX(CASE WHEN prop_mode = 'EME' AND m.dxcc IN (" + modes.join(",") + ") THEN " + innerCase + " ELSE 0 END) as 'EME' "
                    + sqlPart
                    + excludePart +
                    "GROUP BY  1,2) "
                    "SELECT * FROM ( "
                    "SELECT 0 column_idx, "
                    "       '" + tr("TOTAL Worked") + "',  "
                    "       count(DISTINCT " + uniqColumns + "), "
                    "       NULL '160m', "
                    "       NULL '80m', "
                    "       NULL '40m', "
                    "       NULL '30m', "
                    "       NULL '20m', "
                    "       NULL '17m', "
                    "       NULL '15m', "
                    "       NULL '12m', "
                    "       NULL '10m', "
                    "       NULL '6m', "
                    "       NULL '2m', "
                    "       NULL '70cm', "
                    "       NULL 'SAT', "
                    "       NULL 'EME' "
                    "FROM contacts c, modes m "
                    "WHERE c.mode = m.name "
                    "      AND c.station_callsign = '" + ui->myCallComboBox->currentText() + "' "
                    "      AND m.dxcc IN (" + modes.join(",") + ") "
                    + excludePart +
                    "UNION ALL "
                    "SELECT 0 column_idx, "
                    "       '" + tr("TOTAL Confirmed") + "',  "
                    "       count(DISTINCT " + uniqColumns + "), "
                    "       NULL '160m', "
                    "       NULL '80m', "
                    "       NULL '40m', "
                    "       NULL '30m', "
                    "       NULL '20m', "
                    "       NULL '17m', "
                    "       NULL '15m', "
                    "       NULL '12m', "
                    "       NULL '10m', "
                    "       NULL '6m', "
                    "       NULL '2m', "
                    "       NULL '70cm', "
                    "       NULL 'SAT', "
                    "       NULL 'EME' "
                    "FROM contacts c, modes m "
                    "WHERE (" + confirmed.join("or") + ") "
                    "      AND c.mode = m.name "
                    "      AND m.dxcc IN (" + modes.join(",") + ") "
                    "      AND c.station_callsign = '" + ui->myCallComboBox->currentText() + "' "
                    + excludePart +
                    "UNION ALL "
                    "SELECT 1 column_idx, "
                    "       '" + tr("Confirmed") + "', NULL prefix, "
                    "       SUM(CASE WHEN a.'160m' > 1 THEN 1 ELSE 0 END) '160m',  "
                    "       SUM(CASE WHEN a.'80m' > 1 THEN 1 ELSE 0 END) '80m',  "
                    "       SUM(CASE WHEN a.'40m' > 1 THEN 1 ELSE 0 END) '40m',  "
                    "       SUM(CASE WHEN a.'30m' > 1 THEN 1 ELSE 0 END) '30m',  "
                    "       SUM(CASE WHEN a.'20m' > 1 THEN 1 ELSE 0 END) '20m',  "
                    "       SUM(CASE WHEN a.'17m' > 1 THEN 1 ELSE 0 END) '17m',  "
                    "       SUM(CASE WHEN a.'15m' > 1 THEN 1 ELSE 0 END) '15m',  "
                    "       SUM(CASE WHEN a.'12m' > 1 THEN 1 ELSE 0 END) '12m',  "
                    "       SUM(CASE WHEN a.'10m' > 1 THEN 1 ELSE 0 END) '10m',  "
                    "       SUM(CASE WHEN a.'6m' > 1 THEN 1 ELSE 0 END) '6m',  "
                    "       SUM(CASE WHEN a.'2m' > 1 THEN 1 ELSE 0 END) '2m',  "
                    "       SUM(CASE WHEN a.'70cm' > 1 THEN 1 ELSE 0 END) '70cm',  "
                    "       SUM(CASE WHEN a.'SAT' > 1 THEN 1 ELSE 0 END) 'SAT',  "
                    "       SUM(CASE WHEN a.'EME' > 1 THEN 1 ELSE 0 END) 'EME'  "
                    "FROM dxcc_summary a "
                    "GROUP BY 1 "
                    "UNION ALL "
                    "SELECT 2 column_idx, "
                    "       '" + tr("Worked") + "', NULL prefix, "
                    "       SUM(CASE WHEN a.'160m' > 0 THEN 1 ELSE 0 END) '160m',  "
                    "       SUM(CASE WHEN a.'80m' > 0 THEN 1 ELSE 0 END) '80m',  "
                    "       SUM(CASE WHEN a.'40m' > 0 THEN 1 ELSE 0 END) '40m',  "
                    "       SUM(CASE WHEN a.'30m' > 0 THEN 1 ELSE 0 END) '30m',  "
                    "       SUM(CASE WHEN a.'20m' > 0 THEN 1 ELSE 0 END) '20m',  "
                    "       SUM(CASE WHEN a.'17m' > 0 THEN 1 ELSE 0 END) '17m',  "
                    "       SUM(CASE WHEN a.'15m' > 0 THEN 1 ELSE 0 END) '15m',  "
                    "       SUM(CASE WHEN a.'12m' > 0 THEN 1 ELSE 0 END) '12m',  "
                    "       SUM(CASE WHEN a.'10m' > 0 THEN 1 ELSE 0 END) '10m',  "
                    "       SUM(CASE WHEN a.'6m' > 0 THEN 1 ELSE 0 END) '6m',  "
                    "       SUM(CASE WHEN a.'2m' > 0 THEN 1 ELSE 0 END) '2m',  "
                    "       SUM(CASE WHEN a.'70cm' > 0 THEN 1 ELSE 0 END) '70cm',  "
                    "       SUM(CASE WHEN a.'SAT' > 0 THEN 1 ELSE 0 END) 'SAT',  "
                    "       SUM(CASE WHEN a.'EME' > 0 THEN 1 ELSE 0 END) 'EME'  "
                    "FROM dxcc_summary a "
                    "GROUP BY 1 "
                    "UNION ALL "
                    "SELECT 3 column_idx,  "
                    "       col1, col2, "
                    "       SUM(d.'160m') '160m',  "
                    "       SUM(d.'80m') '80m',  "
                    "       SUM(d.'40m') '40m',  "
                    "       SUM(d.'30m') '30m',  "
                    "       SUM(d.'20m') '20m',  "
                    "       SUM(d.'17m') '17m',  "
                    "       SUM(d.'15m') '15m',  "
                    "       SUM(d.'12m') '12m',  "
                    "       SUM(d.'10m') '10m',  "
                    "       SUM(d.'6m') '6m',  "
                    "       SUM(d.'2m') '2m',  "
                    "       SUM(d.'70cm') '70cm',  "
                    "       SUM(d.'SAT') 'SAT',  "
                    "       SUM(d.'EME') 'EME'  "
                    "       from dxcc_summary d "
                    "GROUP BY 2,3 "
                    ") "
                    "ORDER BY 1,2 ");

    qDebug(runtime) << detailedViewModel->query().lastQuery();
    detailedViewModel->setHeaderData(1, Qt::Horizontal, "");
    detailedViewModel->setHeaderData(2, Qt::Horizontal, "");

    ui->awardTableView->setModel(detailedViewModel);
    ui->awardTableView->setColumnHidden(0,true);
}
