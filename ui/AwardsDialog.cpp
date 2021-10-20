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

    QMap<QString, QString> supportedAwards;
    supportedAwards["dxcc"] = tr("DXCC");

    ui->awardComboBox->addItem(tr("DXCC"), QVariant("dxcc"));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Done"));

    ui->awardTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

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

    if ( awardSelected == "dxcc" )
    {
        QStringList confirmed("1=2 ");
        QStringList modes("'NONE'");

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

        QString innerCase = " CASE WHEN (" + confirmed.join("or") + ") THEN 2 ELSE 1 END ";

        detailedViewModel->setQuery(
                    "WITH dxcc_summary AS ( "
                    "SELECT  d.name, d.prefix, "
                    "    MAX(CASE WHEN band = '160m' THEN " + innerCase + " ELSE 0 END) as '160m', "
                    "    MAX(CASE WHEN band = '80m'  THEN " + innerCase + " ELSE 0 END) as '80m', "
                    "    MAX(CASE WHEN band = '40m'  THEN " + innerCase + " ELSE 0 END) as '40m', "
                    "    MAX(CASE WHEN band = '30m'  THEN " + innerCase + " ELSE 0 END) as '30m', "
                    "    MAX(CASE WHEN band = '20m'  THEN " + innerCase + " ELSE 0 END) as '20m', "
                    "    MAX(CASE WHEN band = '17m'  THEN " + innerCase + " ELSE 0 END) as '17m', "
                    "    MAX(CASE WHEN band = '15m'  THEN " + innerCase + " ELSE 0 END) as '15m', "
                    "    MAX(CASE WHEN band = '12m'  THEN " + innerCase + " ELSE 0 END) as '12m', "
                    "    MAX(CASE WHEN band = '10m'  THEN " + innerCase + " ELSE 0 END) as '10m', "
                    "    MAX(CASE WHEN band = '6m'   THEN " + innerCase + " ELSE 0 END) as '6m', "
                    "    MAX(CASE WHEN band = '2m'   THEN " + innerCase + " ELSE 0 END) as '2m', "
                    "    MAX(CASE WHEN band = '70cm' THEN " + innerCase + " ELSE 0 END) as '70cm', "
                    "    MAX(CASE WHEN prop_mode = 'SAT' THEN " + innerCase + " ELSE 0 END) as 'SAT', "
                    "    MAX(CASE WHEN prop_mode = 'EME' THEN " + innerCase + " ELSE 0 END) as 'EME' "
                    "FROM contacts c, dxcc_entities d, modes m  "
                    "WHERE c.dxcc = d.id "
                    "      AND c.mode = m.name "
                    "      AND c.station_callsign = '" + ui->myCallComboBox->currentText() + "' "
                    "      AND m.dxcc IN (" + modes.join(",") + ") "
                    "GROUP BY  1,2) "
                    "SELECT * FROM ( "
                    "SELECT 0 column_idx, "
                    "       '" + tr("TOTAL Worked") + "',  "
                    "       count(DISTINCT c.dxcc), "
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
                    "UNION ALL "
                    "SELECT 0 column_idx, "
                    "       '" + tr("TOTAL Confirmed") + "',  "
                    "       count(DISTINCT c.dxcc), "
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
                    "       a.name, a.prefix,  "
                    "       SUM(a.'160m') '160m',  "
                    "       SUM(a.'80m') '80m',  "
                    "       SUM(a.'40m') '40m',  "
                    "       SUM(a.'30m') '30m',  "
                    "       SUM(a.'20m') '20m',  "
                    "       SUM(a.'17m') '17m',  "
                    "       SUM(a.'15m') '15m',  "
                    "       SUM(a.'12m') '12m',  "
                    "       SUM(a.'10m') '10m',  "
                    "       SUM(a.'6m') '6m',  "
                    "       SUM(a.'2m') '2m',  "
                    "       SUM(a.'70cm') '70cm',  "
                    "       SUM(a.'SAT') 'SAT',  "
                    "       SUM(a.'EME') 'EME'  "
                    "       from dxcc_summary a "
                    "GROUP BY a.name, a.prefix "
                    ") "
                    "ORDER BY 1,2 "
                    );
        qDebug(runtime) << detailedViewModel->query().lastQuery();
        detailedViewModel->setHeaderData(1, Qt::Horizontal, "");
        detailedViewModel->setHeaderData(2, Qt::Horizontal, "");
    }

    ui->awardTableView->setModel(detailedViewModel);
    ui->awardTableView->setColumnHidden(0,true);
}
