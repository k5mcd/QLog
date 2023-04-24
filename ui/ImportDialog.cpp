#include <QFileDialog>
#include <QMessageBox>
#include "ImportDialog.h"
#include "ui_ImportDialog.h"
#include "logformat/LogFormat.h"
#include "core/debug.h"
#include "data/StationProfile.h"
#include "core/Gridsquare.h"
#include "data/RigProfile.h"
#include "data/Data.h"

MODULE_IDENTIFICATION("qlog.ui.importdialog");

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    ui->allCheckBox->setChecked(true);
    ui->startDateEdit->setDisplayFormat(locale.formatDateShortWithYYYY());
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDisplayFormat(locale.formatDateShortWithYYYY());
    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    //ui->progressBar->setDisabled(true);

    QStringList rigs = RigProfilesManager::instance()->profileNameList();
    QStringListModel* rigModel = new QStringListModel(rigs, this);
    ui->rigSelect->setModel(rigModel);
    if (!ui->rigSelect->currentText().isEmpty())
    {
        ui->rigCheckBox->setChecked(true);
        ui->rigSelect->setCurrentText(RigProfilesManager::instance()->getCurProfile1().profileName);
    }

    QStringList profiles = StationProfilesManager::instance()->profileNameList();
    QStringListModel* profileModel = new QStringListModel(profiles, this);
    ui->profileSelect->setModel(profileModel);
    if (!ui->profileSelect->currentText().isEmpty())
    {
        ui->profileSelect->setCurrentText(StationProfilesManager::instance()->getCurProfile1().profileName);
        ui->profileCheckBox->setChecked(true);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("&Import"));
}

void ImportDialog::browse() {
    FCT_IDENTIFICATION;

    QString filename = QFileDialog::getOpenFileName(this, tr("Select File"), "", "*." + ui->typeSelect->currentText().toLower());
    ui->fileEdit->setText(filename);
}

void ImportDialog::toggleAll() {
    FCT_IDENTIFICATION;

    ui->startDateEdit->setEnabled(!ui->allCheckBox->isChecked());
    ui->endDateEdit->setEnabled(!ui->allCheckBox->isChecked());
}

void ImportDialog::toggleMyProfile()
{
    FCT_IDENTIFICATION;

    ui->profileSelect->setEnabled(ui->profileCheckBox->isChecked());
}
void ImportDialog::toggleMyRig()
{
    FCT_IDENTIFICATION;

    ui->rigSelect->setEnabled(ui->rigCheckBox->isChecked());
}

void ImportDialog::toggleComment()
{
    FCT_IDENTIFICATION;

    ui->commentEdit->setEnabled(ui->commentCheckBox->isChecked());
    ui->commentEdit->setToolTip(tr("The value is used when an input record does not contain the value") + "<br/>"
                                + "<b>" + tr("Comment") + ":</b> " + ui->commentEdit->text() + "<br/>");
}

void ImportDialog::computeProgress(qint64 position)
{
    FCT_IDENTIFICATION;

    int progress = (int)(position * 100 / size);
    ui->progressBar->setValue(progress);
    QCoreApplication::processEvents();
}

void ImportDialog::stationProfileTextChanged(QString newProfileName)
{
    FCT_IDENTIFICATION;

    selectedStationProfile = StationProfilesManager::instance()->getProfile(newProfileName);
    ui->profileSelect->setToolTip(tr("The values below will be used when an input record does not contain the values") + "<br/>"
                                  + selectedStationProfile.toHTMLString());
}

void ImportDialog::rigProfileTextChanged(QString newProfileName)
{
    FCT_IDENTIFICATION;
    ui->rigSelect->setToolTip(tr("The values below will be used when an input record does not contain the values") + "<br/>"
                                  + RigProfilesManager::instance()->getProfile(newProfileName).toHTMLString());
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
                                  tr("<p>Do you want to import duplicate QSO?</p>%1 %2").arg(inLogQSO,importedQSO),
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

    if ( ui->fileEdit->text().isEmpty() )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                             QMessageBox::tr("Filename is empty"));
        return;
    }

    QFile file(ui->fileEdit->text());
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&file);

    size = file.size();

    QMap<QString, QString> defaults;

    if (ui->rigCheckBox->isChecked())
    {
        defaults["my_rig_intl"] = ui->rigSelect->currentText();
    }

    if (ui->commentCheckBox->isChecked())
    {
        defaults["comment_intl"] = ui->commentEdit->text();
    }

    if ( ui->profileCheckBox->isChecked()
         && selectedStationProfile != StationProfile() )
    {
        if ( !selectedStationProfile.callsign.isEmpty() )
        {
            defaults["station_callsign"] = selectedStationProfile.callsign.toUpper();
        }

        if ( !selectedStationProfile.locator.isEmpty() )
        {
            defaults["my_gridsquare"] = selectedStationProfile.locator.toUpper();
        }

        if ( !selectedStationProfile.operatorName.isEmpty() )
        {
            defaults["my_name_intl"] = selectedStationProfile.operatorName;
        }

        if ( !selectedStationProfile.qthName.isEmpty() )
        {
            defaults["my_city_intl"] = selectedStationProfile.qthName;
        }

        if ( !selectedStationProfile.iota.isEmpty() )
        {
            defaults["my_iota"] = Data::removeAccents(selectedStationProfile.iota.toUpper());
        }

        if ( !selectedStationProfile.sota.isEmpty() )
        {
            defaults["my_sota_ref"] = Data::removeAccents(selectedStationProfile.sota.toUpper());
        }

        if ( !selectedStationProfile.sig.isEmpty() )
        {
            defaults["my_sig_intl"] = Data::removeAccents(selectedStationProfile.sig);
        }

        if ( !selectedStationProfile.sigInfo.isEmpty() )
        {
            defaults["my_sig_info_intl"] = selectedStationProfile.sigInfo;
        }

        if ( !selectedStationProfile.vucc.isEmpty() )
        {
            defaults["my_vucc_grids"] = selectedStationProfile.vucc.toUpper();
        }

        if ( !selectedStationProfile.vucc.isEmpty() )
        {
            defaults["my_wwff_ref"] = Data::removeAccents(selectedStationProfile.vucc.toUpper());
        }

        if ( !selectedStationProfile.pota.isEmpty() )
        {
            defaults["my_pota_ref"] = Data::removeAccents(selectedStationProfile.pota.toUpper());
        }
    }

    LogFormat* format = LogFormat::open(ui->typeSelect->currentText(), in);

    if (!format) {
        qCritical() << "unknown log format";
        return;
    }

    format->setDefaults(defaults);
    format->setUpdateDxcc(ui->updateDxccCheckBox->isChecked());

    if (!ui->allCheckBox->isChecked()) {
        format->setDateRange(ui->startDateEdit->date(), ui->endDateEdit->date());
    }

    format->setDuplicateQSOCallback(showDuplicateDialog);

    connect(format, &LogFormat::importPosition, this, &ImportDialog::computeProgress);

    ui->buttonBox->setEnabled(false);
    ui->fileEdit->setEnabled(false);
    ui->typeSelect->setEnabled(false);
    ui->browseButton->setEnabled(false);
    ui->startDateEdit->setEnabled(false);
    ui->endDateEdit->setEnabled(false);
    ui->allCheckBox->setEnabled(false);
    ui->profileCheckBox->setEnabled(false);
    ui->profileSelect->setEnabled(false);
    ui->rigCheckBox->setEnabled(false);
    ui->rigSelect->setEnabled(false);
    ui->commentCheckBox->setEnabled(false);
    ui->commentEdit->setEnabled(false);
    ui->updateDxccCheckBox->setEnabled(false);

    QString s;
    QTextStream out(&s);
    unsigned long errors = 0L;
    unsigned long warnings = 0L;

    int count = format->runImport(out, &warnings, &errors);

    QString report = QObject::tr("<b>Imported</b>: %n contact(s)", "", count) + "<br/>" +
                     QObject::tr("<b>Warning(s)</b>: %n", "", warnings) + "<br/>" +
                     QObject::tr("<b>Error(s)</b>: %n", "", errors);

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Import Result"));
    msgBox.setText(report);
    msgBox.setDetailedText(s);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.exec();

    qCDebug(runtime).noquote() << s;

    accept();
}

ImportDialog::~ImportDialog()
{
    FCT_IDENTIFICATION;

    delete ui;
}
