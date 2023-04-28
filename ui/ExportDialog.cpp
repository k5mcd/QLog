#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "ui/ExportDialog.h"
#include "ui_ExportDialog.h"
#include "logformat/LogFormat.h"
#include "core/debug.h"
#include "models/SqlListModel.h"
#include "data/StationProfile.h"

MODULE_IDENTIFICATION("qlog.ui.exportdialog");

ExportDialog::ExportDialog(QWidget *parent) :
   ExportDialog(QList<QSqlRecord>(), parent)
{
    FCT_IDENTIFICATION;

    this->setWindowTitle(tr("Export QSOs"));

    ui->myCallsignComboBox->setModel(new SqlListModel("SELECT DISTINCT UPPER(station_callsign) FROM contacts ORDER BY station_callsign", ""));
    ui->myCallsignComboBox->setCurrentText(StationProfilesManager::instance()->getCurProfile1().callsign.toUpper());
    ui->myGridComboBox->setModel(new SqlListModel("SELECT DISTINCT UPPER(my_gridsquare) FROM contacts WHERE station_callsign ='"
                                                + ui->myCallsignComboBox->currentText()
                                                + "' ORDER BY my_gridsquare", ""));
    ui->startDateEdit->setDisplayFormat(locale.formatDateShortWithYYYY());
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDisplayFormat(locale.formatDateShortWithYYYY());
    ui->endDateEdit->setDate(QDate::currentDate().addDays(1));
}

ExportDialog::ExportDialog(const QList<QSqlRecord>& qsos, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog),
    qsos4export(qsos)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("&Export"));

    if ( qsos4export.size() > 0 )
    {
        ui->filterGroup->setVisible(false);
        adjustSize();
    }
}

void ExportDialog::browse()
{
    FCT_IDENTIFICATION;

    QString filename = QFileDialog::getSaveFileName(this);
    ui->fileEdit->setText(filename);
}

void ExportDialog::toggleDateRange()
{
    FCT_IDENTIFICATION;

    ui->startDateEdit->setEnabled(ui->dateRangeCheckBox->isChecked());
    ui->endDateEdit->setEnabled(ui->dateRangeCheckBox->isChecked());
}

void ExportDialog::toggleMyCallsign()
{
    FCT_IDENTIFICATION;
    ui->myCallsignComboBox->setEnabled(ui->myCallsignCheckBox->isChecked());
}

void ExportDialog::toggleMyGridsquare()
{
    FCT_IDENTIFICATION;
    ui->myGridComboBox->setEnabled(ui->myGridCheckBox->isChecked());
}

void ExportDialog::runExport()
{
    FCT_IDENTIFICATION;

    if ( ui->fileEdit->text().isEmpty() )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                             QMessageBox::tr("Filename is empty"));
        return;
    }

    QFile file(ui->fileEdit->text());

    if ( ! file.open(QFile::WriteOnly | QFile::Text) )
    {
        QMessageBox::critical(nullptr, QMessageBox::tr("QLog Error"),
                             QMessageBox::tr("Cannot write to the file"));
        return;
    }

    QTextStream out(&file);

    LogFormat* format = LogFormat::open(ui->typeSelect->currentText(), out);

    if (!format)
    {
        qCritical() << "unknown log format";
        return;
    }

    if ( ui->dateRangeCheckBox->isChecked() )
    {
        format->setFilterDateRange(ui->startDateEdit->date(), ui->endDateEdit->date());
    }

    if ( ui->myCallsignCheckBox->isChecked() )
    {
        format->setFilterMyCallsign(ui->myCallsignComboBox->currentText());
    }

    if ( ui->myGridCheckBox->isChecked() )
    {
        format->setFilterMyGridsquare(ui->myGridComboBox->currentText());
    }

    long count = 0L;

    connect(format, &LogFormat::exportProgress, this, &ExportDialog::setProgress);

    ui->buttonBox->setEnabled(false);
    ui->fileEdit->setEnabled(false);
    ui->typeSelect->setEnabled(false);
    ui->browseButton->setEnabled(false);
    ui->startDateEdit->setEnabled(false);
    ui->endDateEdit->setEnabled(false);
    ui->dateRangeCheckBox->setEnabled(false);
    ui->myCallsignCheckBox->setEnabled(false);
    ui->myCallsignComboBox->setEnabled(false);
    ui->myGridCheckBox->setEnabled(false);
    ui->myGridComboBox->setEnabled(false);

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
                         QMessageBox::tr("Exported %n contact(s).", "", count));

    accept();
}

void ExportDialog::myCallsignChanged(const QString &myCallsign)
{
    FCT_IDENTIFICATION;

    ui->myGridComboBox->setModel(new SqlListModel("SELECT DISTINCT UPPER(my_gridsquare) FROM contacts WHERE station_callsign ='"
                                                  + myCallsign + "' ORDER BY my_gridsquare", ""));
}

void ExportDialog::setProgress(float progress)
{
    FCT_IDENTIFICATION;

    ui->progressBar->setValue(progress);
    QCoreApplication::processEvents();
}

ExportDialog::~ExportDialog()
{
    FCT_IDENTIFICATION;

    delete ui;
}
