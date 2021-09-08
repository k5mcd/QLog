#include <QTextStream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTemporaryFile>
#include <QSettings>
#include "LotwDialog.h"
#include "ui_LotwDialog.h"
#include "logformat/AdiFormat.h"
#include "core/Lotw.h"
#include "core/debug.h"
#include "ui/QSLImportStatDialog.h"
#include "models/SqlListModel.h"

MODULE_IDENTIFICATION("qlog.ui.lotwdialog");

LotwDialog::LotwDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LotwDialog)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    ui->qslRadioButton->setChecked(true);
    ui->qsoRadioButton->setChecked(false);

    QSettings settings;
    if (settings.value("lotw/last_update").isValid()) {
        QDate last_update = settings.value("lotw/last_update").toDate();
        ui->dateEdit->setDate(last_update);
    }
    else {
        ui->dateEdit->setDate(QDateTime::currentDateTimeUtc().date());
    }

    ui->stationCombo->setModel(new SqlListModel("SELECT DISTINCT UPPER(station_callsign) FROM contacts ORDER BY station_callsign", ""));
}

void LotwDialog::download() {
    FCT_IDENTIFICATION;

    QProgressDialog* dialog = new QProgressDialog(tr("Downloading from LotW"), tr("Cancel"), 0, 0, this);
    dialog->setWindowModality(Qt::WindowModal);
    dialog->setRange(0, 0);
    dialog->show();

    bool qsl = ui->qslRadioButton->isChecked();

    Lotw* lotw = new Lotw(dialog);
    connect(lotw, &Lotw::updateProgress, dialog, &QProgressDialog::setValue);

    connect(lotw, &Lotw::updateStarted, [dialog] {
        dialog->setLabelText(tr("Processing LotW QSLs"));
        dialog->setRange(0, 100);
    });

    connect(lotw, &Lotw::updateComplete, [this, dialog, qsl](QSLMergeStat stats) {
        if (qsl) {
            QSettings settings;
            settings.setValue("lotw/last_update", QDateTime::currentDateTimeUtc().date());
        }
        dialog->close();


        QSLImportStatDialog statDialog(stats);
        statDialog.exec();

        qCDebug(runtime) << "New QSLs: " << stats.newQSLs;
        qCDebug(runtime) << "Unmatched QSLs: " << stats.unmatchedQSLs;
    });

    connect(lotw, &Lotw::updateFailed, [this, dialog]() {
        dialog->close();
        QMessageBox::critical(this, tr("QLog Error"), tr("LotW Update failed."));
    });

    lotw->update(ui->dateEdit->date(), ui->qsoRadioButton->isChecked(), ui->stationCombo->currentText().toUpper());

    dialog->exec();
}

void LotwDialog::upload() {
    FCT_IDENTIFICATION;

    QTemporaryFile file;
    file.open();

    QSqlQuery query("SELECT callsign, freq, band, freq_rx, mode, submode, start_time, prop_mode, sat_name, station_callsign, operator, rst_sent, rst_rcvd, my_state, my_cnty, my_vucc_grids FROM contacts WHERE (lotw_qsl_sent <> 'Y' OR lotw_qsl_sent is NULL) AND (prop_mode NOT IN ('INTERNET', 'RPT', 'ECH', 'IRL') OR prop_mode IS NULL)");

    QTextStream stream(&file);
    AdiFormat adi(stream);

    int count = 0;

    while (query.next()) {
        QSqlRecord record = query.record();
        adi.exportContact(record);
        count++;
    }

    stream.flush();

    if (count > 0) {
        //QProcess::execute("tqsl -d -q -u " + file.fileName());
        // how to make a backup of sended tsql file?
        // path to tqsl ?
        int ErrorCode = QProcess::execute("tqsl -d -q -o /home/foldynl/pokus.adif " + file.fileName());

        /* list of Error Codes: http://www.arrl.org/command-1 */
        QString ErrorString;

        switch ( ErrorCode )
        {
        case 0: // Success
            break;

        case 1: // Cancelled by user
            ErrorString = tr("Upload cancelled by user");
            break;

        case 2: // Rejected by LoTW
            ErrorString = tr("Upload rejected by LoTW");
            break;

        case 3: // Unexpected response from TQSL server
            ErrorString = tr("Unexpected response from TQSL server");
            break;

        case 4: // TQSL error
            ErrorString = tr("TQSL utility error");
            break;

        case 5: // TQSLlib error
            ErrorString = tr("TQSLlib error");
            break;

        case 6: // Unable to open input file
            ErrorString = tr("Unable to open input file");
            break;

        case 7: // Unable to open output file
            ErrorString = tr("Unable to open output file");
            break;

        case 8: // All QSOs were duplicates or out of date range
            ErrorString = tr("All QSOs were duplicates or out of date range");
            break;

        case 9: // Some QSOs were duplicates or out of date range
            ErrorString = tr("Some QSOs were duplicates or out of date range");
            break;

        case 10: // Command syntax error
            ErrorString = tr("Command syntax error");
            break;

        case 11: // LoTW Connection error (no network or LoTW is unreachable)
            ErrorString = tr("LoTW Connection error (no network or LoTW is unreachable)");
            break;

        default:
            ErrorString = tr("Unexpected Error from TQSL");
        }

        if ( ErrorCode == 0 )
        {
            QMessageBox::information(this, tr("QLog Information"), tr("%n QSO(s) uploaded.", "", count));
            //update contact set lotw_sent = y, lotwsdate curr_date
        }
        else
        {
            QMessageBox::critical(this, tr("LoTW Error"), ErrorString);
        }
    }
    else {
        QMessageBox::information(this, tr("QLog Information"), tr("No QSOs found to upload."));
    }
}

LotwDialog::~LotwDialog()
{
    FCT_IDENTIFICATION;

    delete ui;
}
