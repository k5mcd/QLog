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

    ui->stationCombo->addItem(settings.value("station/callsign").toString().toUpper());
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

        QMessageBox::information(this, tr("QLog Info"),
                                 tr("LotW Update completed.") + "\n\n" +
                                 tr("QSOs checked: %n", "", stats.qsos_checked) + "\n" +
                                 tr("New QSLs received: %n", "", stats.qsos_updated) + "\n" +
                                 tr("Unmatched QSOs: %n", "", stats.qsos_unmatched) + "\n" +
                                 tr("Error QSOs: %n", "", stats.qsos_errors)
                                 );
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

    QSqlQuery query("SELECT * FROM contacts WHERE NOT lotw_qsl_sent = 'Y'");

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
        QProcess::execute("tqsl -d -q -u " + file.fileName());
        QMessageBox::information(this, tr("QLog Information"), tr("%n QSO(s) uploaded.", "", count));
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
