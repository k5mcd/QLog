#include <QProgressDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QNetworkReply>

#include "ClublogDialog.h"
#include "ui_ClublogDialog.h"
#include "core/debug.h"
#include "core/ClubLog.h"
#include "models/SqlListModel.h"
#include "logformat/AdiFormat.h"
#include "ui/LotwShowUploadDialog.h"

MODULE_IDENTIFICATION("qlog.ui.clublogdialog");

ClublogDialog::ClublogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClublogDialog)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    /* Upload */

    ui->myCallsignCombo->setModel(new SqlListModel("SELECT DISTINCT UPPER(station_callsign) FROM contacts ORDER BY station_callsign", ""));
    ui->myGridCombo->setModel(new SqlListModel("SELECT DISTINCT UPPER(my_gridsquare) FROM contacts WHERE station_callsign ='"
                                                + ui->myCallsignCombo->currentText()
                                                + "' ORDER BY my_gridsquare", ""));

}

ClublogDialog::~ClublogDialog()
{
    delete ui;
}

void ClublogDialog::upload()
{
    FCT_IDENTIFICATION;

    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);

    AdiFormat adi(stream);
    QLocale locale;
    QString QSOList;
    int count = 0;

    /* https://clublog.freshdesk.com/support/solutions/articles/54905-how-to-upload-logs-directly-into-club-log */
    /* https://clublog.freshdesk.com/support/solutions/articles/53202-which-adif-fields-does-club-log-use- */
    QString query_string = "SELECT start_time, "
                           "       qsl_rdate, qsl_sdate, "
                           "       callsign, operator, "
                           "       mode, band, band_rx, freq, "
                           "       qsl_rcvd, lotw_qsl_rcvd, "
                           "       qsl_sent, dxcc, "
                           "       prop_mode, credit_granted, "
                           "       rst_sent, rst_rcvd, notes, "
                           "       gridsquare, vucc_grids, sat_name ";
    QString query_from   = "FROM contacts ";
    QString query_where =  "WHERE (clublog_qso_upload_status <> 'Y' OR clublog_qso_upload_status is NULL) ";
    QString query_order = " ORDER BY start_time ";


    if ( !ui->myCallsignCombo->currentText().isEmpty() )
    {
        query_where.append(" AND station_callsign = '" + ui->myCallsignCombo->currentText() + "'");
    }

    if ( !ui->myGridCombo->currentText().isEmpty() )
    {
        query_where.append(" AND my_gridsquare = '" + ui->myGridCombo->currentText() + "'");
    }

    query_string = query_string + query_from + query_where + query_order;

    qCDebug(runtime) << query_string;

    QSqlQuery query(query_string);

    adi.exportStart();

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
        /* TODO: rename it to ShowUploadDialog */
        LotwShowUploadDialog showDialog(QSOList);

        if ( showDialog.exec() == QDialog::Accepted )
        {
            QProgressDialog* dialog = new QProgressDialog(tr("Uploading to Clublog"), tr("Cancel"), 0, 0, this);
            dialog->setWindowModality(Qt::WindowModal);
            dialog->setRange(0, 0);
            dialog->show();

            ClubLog *clublog = new ClubLog(dialog);

            connect(clublog, &ClubLog::uploadOK, [this, dialog, query_where, count](QString msg)
            {
                dialog->done(0);
                qCDebug(runtime) << "Clublog Upload OK: " << msg;
                QMessageBox::information(this, tr("QLog Information"), tr("%n QSO(s) uploaded.", "", count));
                QString query_string = "UPDATE contacts "
                                       "SET clublog_qso_upload_status='Y', clublog_qso_upload_date = strftime('%Y-%m-%d',DATETIME('now', 'utc')) "
                        + query_where;

                qCDebug(runtime) << query_string;

                QSqlQuery query_update(query_string);
                query_update.exec();
            });

            connect(clublog, &ClubLog::uploadError, [this, dialog](QString msg)
            {
                dialog->done(1);
                qCInfo(runtime) << "Clublog Upload Error: " << msg;
                QMessageBox::warning(this, tr("QLog Warning"), tr("Cannot upload the QSO: ") + msg);
            });

            QNetworkReply *reply = clublog->uploadAdif(data);

            connect(dialog, &QProgressDialog::canceled, [reply]()
            {
                qCDebug(runtime)<< "Operation canceled";
                if ( reply )
                {
                    reply->abort();
                    reply->deleteLater();
                }
            });

        }
    }
    else
    {
        QMessageBox::information(this, tr("QLog Information"), tr("No QSOs found to upload."));
    }
}



void ClublogDialog::uploadCallsignChanged(QString my_callsign)
{
    FCT_IDENTIFICATION;

    ui->myGridCombo->setModel(new SqlListModel("SELECT DISTINCT UPPER(my_gridsquare) FROM contacts WHERE station_callsign ='" + my_callsign + "' ORDER BY my_gridsquare", ""));

}
