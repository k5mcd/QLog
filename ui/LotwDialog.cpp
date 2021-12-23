#include <QTextStream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>
#include <QNetworkReply>

#include "LotwDialog.h"
#include "ui_LotwDialog.h"
#include "logformat/AdiFormat.h"
#include "core/Lotw.h"
#include "core/debug.h"
#include "ui/QSLImportStatDialog.h"
#include "models/SqlListModel.h"
#include "ui/LotwShowUploadDialog.h"

MODULE_IDENTIFICATION("qlog.ui.lotwdialog");

LotwDialog::LotwDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LotwDialog)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    /* Upload */

    ui->myCallsignCombo->setModel(new SqlListModel("SELECT DISTINCT UPPER(station_callsign) FROM contacts ORDER BY station_callsign", ""));
    ui->myGridCombo->setModel(new SqlListModel("SELECT DISTINCT UPPER(my_gridsquare) FROM contacts WHERE station_callsign ='"
                                                + ui->myCallsignCombo->currentText()
                                                + "' ORDER BY my_gridsquare", ""));
    /* Download */
    ui->qslRadioButton->setChecked(true);
    ui->qsoRadioButton->setChecked(false);

    ui->stationCombo->setModel(new SqlListModel("SELECT DISTINCT UPPER(station_callsign) FROM contacts ORDER BY station_callsign", ""));

    loadDialogState();
}

void LotwDialog::download() {
    FCT_IDENTIFICATION;

    QProgressDialog* dialog = new QProgressDialog(tr("Downloading from LotW"), tr("Cancel"), 0, 0, this);
    dialog->setWindowModality(Qt::WindowModal);
    dialog->setRange(0, 0);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->show();

    bool qsl = ui->qslRadioButton->isChecked();

    Lotw* lotw = new Lotw(dialog);
    connect(lotw, &Lotw::updateProgress, dialog, &QProgressDialog::setValue);

    connect(lotw, &Lotw::updateStarted, this, [dialog] {
        dialog->setLabelText(tr("Processing LotW QSLs"));
        dialog->setRange(0, 100);
    });

    connect(lotw, &Lotw::updateComplete, this, [dialog, qsl](QSLMergeStat stats) {
        if (qsl) {
            QSettings settings;
            settings.setValue("lotw/last_update", QDateTime::currentDateTimeUtc().date());
        }
        dialog->done(QDialog::Accepted);

        QSLImportStatDialog statDialog(stats);
        statDialog.exec();
    });

    connect(lotw, &Lotw::updateFailed, this, [this, dialog](QString error) {
        dialog->done(QDialog::Accepted);
        QMessageBox::critical(this, tr("QLog Error"), tr("LoTW Update failed: ") + error);
    });

    saveDialogState();

    QNetworkReply *reply = lotw->update(ui->dateEdit->date(), ui->qsoRadioButton->isChecked(), ui->stationCombo->currentText().toUpper());

    connect(dialog, &QProgressDialog::canceled, this, [reply]()
    {
        qCDebug(runtime)<< "Operation canceled";
        if ( reply )
        {
            reply->abort();
            reply->deleteLater();
        }
    });
}

void LotwDialog::upload() {
    FCT_IDENTIFICATION;

    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);

    AdiFormat adi(stream);
    QLocale locale;
    QString QSOList;
    int count = 0;

    QString query_string = "SELECT callsign, freq, band, freq_rx, "
                           "       mode, submode, start_time, prop_mode, "
                           "       sat_name, station_callsign, operator, "
                           "       rst_sent, rst_rcvd, my_state, my_cnty, "
                           "       my_vucc_grids "
                           "FROM contacts ";
    QString query_where =  "WHERE (lotw_qsl_sent <> 'Y' OR lotw_qsl_sent is NULL) "
                           "      AND (prop_mode NOT IN ('INTERNET', 'RPT', 'ECH', 'IRL') OR prop_mode IS NULL) ";
    QString query_order = " ORDER BY start_time ";

    saveDialogState();

    if ( !ui->myCallsignCombo->currentText().isEmpty() )
    {
        query_where.append(" AND station_callsign = '" + ui->myCallsignCombo->currentText() + "'");
    }

    if ( !ui->myGridCombo->currentText().isEmpty() )
    {
        query_where.append(" AND my_gridsquare = '" + ui->myGridCombo->currentText() + "'");
    }

    query_string = query_string + query_where + query_order;

    qCDebug(runtime) << query_string;

    QSqlQuery query(query_string);

    while (query.next())
    {
        QSqlRecord record = query.record();

        QSOList.append(" "
                       + record.value("start_time").toDateTime().toTimeSpec(Qt::UTC).toString(locale.dateTimeFormat(QLocale::ShortFormat))
                       + " " + record.value("callsign").toString()
                       + " " + record.value("mode").toString()
                       + "\n");

        adi.exportContact(record);
        count++;
    }

    stream.flush();

    if (count > 0)
    {
        LotwShowUploadDialog showDialog(QSOList);

        if ( showDialog.exec() == QDialog::Accepted )
        {
            Lotw lotw;
            QString ErrorString;

            if ( lotw.uploadAdif(data, ErrorString) == 0 )
            {
                QMessageBox::information(this, tr("QLog Information"), tr("%n QSO(s) uploaded.", "", count));

                query_string = "UPDATE contacts "
                               "SET lotw_qsl_sent='Y', lotw_qslsdate = strftime('%Y-%m-%d',DATETIME('now', 'utc')) "
                               + query_where;

                qCDebug(runtime) << query_string;

                QSqlQuery query_update(query_string);
                query_update.exec();
            }
            else
            {
                QMessageBox::critical(this, tr("LoTW Error"), ErrorString);
            }
        }
    }
    else {
        QMessageBox::information(this, tr("QLog Information"), tr("No QSOs found to upload."));
    }
}

void LotwDialog::uploadCallsignChanged(const QString &my_callsign)
{
    ui->myGridCombo->setModel(new SqlListModel("SELECT DISTINCT UPPER(my_gridsquare) FROM contacts WHERE station_callsign ='" + my_callsign + "' ORDER BY my_gridsquare", ""));
}

void LotwDialog::saveDialogState()
{
    FCT_IDENTIFICATION;
    QSettings settings;
    settings.setValue("lotw/last_mycallsign", ui->myCallsignCombo->currentText());
    settings.setValue("lotw/last_mygrid", ui->myGridCombo->currentText());
    settings.setValue("lotw/last_station", ui->stationCombo->currentText());
    settings.setValue("lotw/last_qsoqsl", ui->qslRadioButton->isChecked());
}

void LotwDialog::loadDialogState()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    ui->myCallsignCombo->setCurrentText(settings.value("lotw/last_mycallsign").toString());
    ui->myGridCombo->setCurrentText(settings.value("lotw/last_mygrid").toString());

    if (settings.value("lotw/last_update").isValid()) {
        QDate last_update = settings.value("lotw/last_update").toDate();
        ui->dateEdit->setDate(last_update);
    }
    else {
        ui->dateEdit->setDate(QDateTime::currentDateTimeUtc().date());
    }

    ui->qslRadioButton->setChecked(settings.value("lotw/last_qsoqsl",0).toBool());
    ui->qsoRadioButton->setChecked(!settings.value("lotw/last_qsoqsl",0).toBool());
}

LotwDialog::~LotwDialog()
{
    FCT_IDENTIFICATION;

    delete ui;
}
