#include <QFileDialog>
#include <QMessageBox>
#include "ImportDialog.h"
#include "ui_ImportDialog.h"
#include "logformat/LogFormat.h"
#include "core/debug.h"
#include "data/StationProfile.h"

MODULE_IDENTIFICATION("qlog.ui.importdialog");

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    QSettings settings;

    ui->allCheckBox->setChecked(true);
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());
    ui->gridEdit->setText(StationProfilesManager::instance()->getCurrent().locator);
    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    //ui->progressBar->setDisabled(true);

    QStringList rigs = settings.value("station/rigs").toStringList();
    QStringListModel* rigModel = new QStringListModel(rigs, this);
    ui->rigSelect->setModel(rigModel);
    if (!ui->rigSelect->currentText().isEmpty()) {
        ui->rigCheckBox->setChecked(true);
    }
}

void ImportDialog::browse() {
    FCT_IDENTIFICATION;

    QString filename = QFileDialog::getOpenFileName(this, "ADIF File", "", "*.adi");
    ui->fileEdit->setText(filename);
}

void ImportDialog::toggleAll() {
    FCT_IDENTIFICATION;

    ui->startDateEdit->setEnabled(!ui->allCheckBox->isChecked());
    ui->endDateEdit->setEnabled(!ui->allCheckBox->isChecked());
}

void ImportDialog::progress(qint64 value) {
    FCT_IDENTIFICATION;

    int progress = (int)(value * 100 / size);
    ui->progressBar->setValue(progress);
    QCoreApplication::processEvents();
}

LogFormat::duplicateQSOBehaviour ImportDialog::showDuplicateDialog(QSqlRecord *imported, QSqlRecord *original)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << *imported << " " << *original;

    LogFormat::duplicateQSOBehaviour ret = LogFormat::ACCEPT_ONE;

    QMessageBox::StandardButton reply;

    QString inLogQSO = tr("<p><b>In-Log QSO:</b></p><p>")
                       + original->value("start_time").toString() + " "
                       + original->value("callsign").toString() + "</p>";

    QString importedQSO = tr("<p><b>Importing:</b></p><p>")
                       + imported->value("start_time").toString() + " "
                       + imported->value("callsign").toString() + "<p> ";

    reply = QMessageBox::question(nullptr,
                                  tr("Duplicate QSO"),
                                  tr("<p>Do you want to import duplicate QSO?</p>%1 %2").arg(inLogQSO).arg(importedQSO),
                                  QMessageBox::Yes|QMessageBox::No|QMessageBox::YesAll|QMessageBox::NoAll);
    switch ( reply )
    {
    case QMessageBox::Yes:
        ret = LogFormat::ACCEPT_ONE;
        break;
    case QMessageBox::YesAll:
        ret = LogFormat::ACCEPT_ALL;
        break;
    case QMessageBox::No:
        ret = LogFormat::SKIP_ONE;
        break;
    case QMessageBox::NoAll:
        ret = LogFormat::SKIP_ALL;
        break;
    default:
        ret = LogFormat::ASK_NEXT;
    }

    qCDebug(runtime) << "ret: " << ret;
    return ret;
}

void ImportDialog::runImport() {
    FCT_IDENTIFICATION;

    QFile file(ui->fileEdit->text());
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&file);

    size = file.size();

    QMap<QString, QString> defaults;

    if (ui->gridCheckBox->isChecked()) {
        defaults["my_gridsquare"] = ui->gridEdit->text();
    }

    if (ui->rigCheckBox->isChecked()) {
        defaults["my_rig"] = ui->rigSelect->currentText();
    }

    if (ui->commentCheckBox->isChecked()) {
        defaults["comment"] = ui->commentEdit->text();
    }

    LogFormat* format = LogFormat::open(ui->typeSelect->currentText(), in);
    format->setDefaults(defaults);
    format->setUpdateDxcc(ui->updateDxccCheckBox->isChecked());

    if (!format) {
        qCritical() << "unknown log format";
        return;
    }

    if (!ui->allCheckBox->isChecked()) {
        format->setDateRange(ui->startDateEdit->date(), ui->endDateEdit->date());
    }

    format->setDuplicateQSOCallback(showDuplicateDialog);

    /*
    QThread* thread = new QThread;
    format->moveToThread(thread);
    */

    connect(format, &LogFormat::progress, this, &ImportDialog::progress);
    /*
    connect(thread, &QThread::started, format, &LogFormat::runImport);
    connect(format, &LogFormat::finished, thread, &QThread::quit);
    connect(format, &LogFormat::finished, format, &LogFormat::deleteLater);
    connect(format, &LogFormat::finished, thread, &QThread::deleteLater);

    thread->start();
    */

    ui->buttonBox->setEnabled(false);
    //format->runImport();
    format->runImport();
    ui->buttonBox->setEnabled(true);

    this->close();
    //ui->statusLabel->setText(tr("Imported %n contacts.", "", count));
}

ImportDialog::~ImportDialog()
{
    FCT_IDENTIFICATION;

    delete ui;
}
