#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "ui/ExportDialog.h"
#include "ui_ExportDialog.h"
#include "logformat/LogFormat.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.exportdialog");

ExportDialog::ExportDialog(QWidget *parent) :
   ExportDialog(QList<QSqlRecord>(), parent)
{
    FCT_IDENTIFICATION;
    this->setWindowTitle(tr("Export All QSOs"));
}

ExportDialog::ExportDialog(const QList<QSqlRecord>& qsos, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog),
    qsos4export(qsos)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    ui->allCheckBox->setChecked(true);
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate().addDays(1));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("&Export"));
}

void ExportDialog::browse() {
    FCT_IDENTIFICATION;

    QString filename = QFileDialog::getSaveFileName(this);
    ui->fileEdit->setText(filename);
}

void ExportDialog::toggleAll() {
    FCT_IDENTIFICATION;

    ui->startDateEdit->setEnabled(!ui->allCheckBox->isChecked());
    ui->endDateEdit->setEnabled(!ui->allCheckBox->isChecked());
}

void ExportDialog::runExport() {
    FCT_IDENTIFICATION;

    if ( ui->fileEdit->text().isEmpty() )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                             QMessageBox::tr("Filename is empty"));
        return;
    }

    QFile file(ui->fileEdit->text());
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&file);

    LogFormat* format = LogFormat::open(ui->typeSelect->currentText(), out);

    if (!format) {
        qCritical() << "unknown log format";
        return;
    }

    if (!ui->allCheckBox->isChecked()) {
        format->setDateRange(ui->startDateEdit->date(), ui->endDateEdit->date());
    }

    int count = 0;

    connect(format, &LogFormat::exportProgress, this, &ExportDialog::setProgress);

    ui->buttonBox->setEnabled(false);
    ui->fileEdit->setEnabled(false);
    ui->typeSelect->setEnabled(false);
    ui->browseButton->setEnabled(false);
    ui->startDateEdit->setEnabled(false);
    ui->endDateEdit->setEnabled(false);
    ui->allCheckBox->setEnabled(false);

    if ( qsos4export.size() > 0 )
    {
        count = format->runExport(qsos4export);
    }
    else
    {
        count = format->runExport();
    }

    delete format;

    QMessageBox::information(nullptr, QMessageBox::tr("QLog Information"),
                         QMessageBox::tr("Exported %n contacts.", "", count));

    accept();
}

void ExportDialog::setProgress(float progress)
{
    FCT_IDENTIFICATION;

    ui->progressBar->setValue(progress);
    QCoreApplication::processEvents();
}

ExportDialog::~ExportDialog() {
    FCT_IDENTIFICATION;

    delete ui;
}
