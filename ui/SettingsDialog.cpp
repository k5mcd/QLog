#include <QSettings>
#include <QStringListModel>
#include <QSqlTableModel>
#include <hamlib/rig.h>
#include <hamlib/rotator.h>
#include <QFileDialog>
#include <QMessageBox>

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "models/RigTypeModel.h"
#include "models/RotTypeModel.h"
#include "../core/GenericCallbook.h"
#include "../core/Lotw.h"
#include "../core/ClubLog.h"
#include "../core/Eqsl.h"
#include "../core/StyleItemDelegate.h"
#include "core/debug.h"
#include "core/CredentialStore.h"
#include "data/StationProfile.h"
#include "data/Data.h"
#include "core/Gridsquare.h"

MODULE_IDENTIFICATION("qlog.ui.settingdialog");

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    profileManager(StationProfilesManager::instance()),
    ui(new Ui::SettingsDialog)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    RigTypeModel* rigTypeModel = new RigTypeModel(this);
    ui->rigModelSelect->setModel(rigTypeModel);

    RotTypeModel* rotTypeModel = new RotTypeModel(this);
    ui->rotModelSelect->setModel(rotTypeModel);

    QStringListModel* rigModel = new QStringListModel();
    ui->rigListView->setModel(rigModel);

    QStringListModel* antModel = new QStringListModel();
    ui->antListView->setModel(antModel);

    QStringListModel* profilesModes = new QStringListModel();
    ui->profilesListView->setModel(profilesModes);

    modeTableModel = new QSqlTableModel(this);
    modeTableModel->setTable("modes");
    modeTableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    modeTableModel->setSort(1, Qt::AscendingOrder);
    modeTableModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
    modeTableModel->setHeaderData(3, Qt::Horizontal, tr("Report"));
    modeTableModel->setHeaderData(4, Qt::Horizontal, tr("DXCC"));
    modeTableModel->setHeaderData(5, Qt::Horizontal, tr("State"));
    ui->modeTableView->setModel(modeTableModel);

    ui->modeTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->modeTableView->hideColumn(0);
    ui->modeTableView->hideColumn(2);
    ui->modeTableView->setItemDelegateForColumn(5,new CheckBoxDelegate(ui->modeTableView));
    ui->modeTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    modeTableModel->select();

    bandTableModel = new QSqlTableModel(this);
    bandTableModel->setTable("bands");
    bandTableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    bandTableModel->setSort(2, Qt::AscendingOrder);
    bandTableModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
    bandTableModel->setHeaderData(2, Qt::Horizontal, tr("Start"));
    bandTableModel->setHeaderData(3, Qt::Horizontal, tr("End"));
    bandTableModel->setHeaderData(4, Qt::Horizontal, tr("State"));
    ui->bandTableView->setModel(bandTableModel);

    ui->bandTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->bandTableView->hideColumn(0);
    ui->bandTableView->setItemDelegateForColumn(4,new CheckBoxDelegate(ui->bandTableView));
    ui->bandTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    bandTableModel->select();

    ui->callsignEdit->setValidator(new QRegularExpressionValidator(Data::callsignRegEx(), this));
    ui->locatorEdit->setValidator(new QRegularExpressionValidator(Gridsquare::gridRegEx(), this));
    ui->vuccEdit->setValidator(new QRegularExpressionValidator(Gridsquare::gridVUCCRegEx(), this));

    iotaCompleter = new QCompleter(Data::instance()->iotaIDList(), this);
    iotaCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    iotaCompleter->setFilterMode(Qt::MatchContains);
    iotaCompleter->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    ui->iotaEdit->setCompleter(iotaCompleter);

    sotaCompleter = new QCompleter(Data::instance()->sotaIDList(), this);
    sotaCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    sotaCompleter->setFilterMode(Qt::MatchStartsWith);
    sotaCompleter->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    ui->sotaEdit->setCompleter(nullptr);

    readSettings();
}

void SettingsDialog::save() {
    FCT_IDENTIFICATION;

    if ( profileManager->profilesList().isEmpty() )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                             QMessageBox::tr("Please, define at least one Station Locations Profile"));
        return;
    }

    writeSettings();
    accept();
}

void SettingsDialog::addRig() {
    FCT_IDENTIFICATION;

    if (ui->rigNameEdit->text().isEmpty()) return;

    QStringListModel* model = (QStringListModel*)ui->rigListView->model();
    QStringList rigs = model->stringList();
    rigs << ui->rigNameEdit->text();
    model->setStringList(rigs);
    ui->rigNameEdit->clear();
}

void SettingsDialog::deleteRig() {
    FCT_IDENTIFICATION;

    foreach (QModelIndex index, ui->rigListView->selectionModel()->selectedRows()) {
        ui->rigListView->model()->removeRow(index.row());
    }
    ui->rigListView->clearSelection();
}

void SettingsDialog::addAnt() {
    FCT_IDENTIFICATION;

    if (ui->antennasEdit->text().isEmpty()) return;

    QStringListModel* model = (QStringListModel*)ui->antListView->model();
    QStringList ants = model->stringList();
    ants << ui->antennasEdit->text();
    model->setStringList(ants);
    ui->antennasEdit->clear();
}

void SettingsDialog::deleteAnt() {
    FCT_IDENTIFICATION;

    foreach (QModelIndex index, ui->antListView->selectionModel()->selectedRows()) {
        ui->antListView->model()->removeRow(index.row());
    }
    ui->antListView->clearSelection();
}

void SettingsDialog::refreshStationProfilesView()
{
    FCT_IDENTIFICATION;
    QStringListModel* model = (QStringListModel*)ui->profilesListView->model();
    QStringList profiles = model->stringList();

    profiles.clear();

    profiles << profileManager->profilesList();

    model->setStringList(profiles);
}
void SettingsDialog::addStationProfile()
{
    FCT_IDENTIFICATION;

    if ( ui->profileEdit->text().isEmpty() )
    {
        ui->profileEdit->setPlaceholderText(tr("Must not be empty"));
        return;
    }

    if ( ui->callsignEdit->text().isEmpty() )
    {
        ui->callsignEdit->setPlaceholderText(tr("Must not be empty"));
        return;
    }

    if ( ui->locatorEdit->text().isEmpty() )
    {
        ui->locatorEdit->setPlaceholderText(tr("Must not be empty"));
        return;
    }

    if ( ! ui->callsignEdit->hasAcceptableInput() )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                             QMessageBox::tr("Callsign has an invalid format"));
        return;
    }

    if ( ! ui->locatorEdit->hasAcceptableInput() )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                             QMessageBox::tr("Locator has an invalid format"));
        return;
    }

    if ( ! ui->vuccEdit->text().isEmpty() )
    {
        if ( ! ui->vuccEdit->hasAcceptableInput() )
        {
            QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                                 QMessageBox::tr("VUCC Locator has an invalid format (must be 2 or 4 locators separated by ',')"));
            return;
        }
    }

    if ( ui->addProfileButton->text() == tr("Modify"))
    {
        ui->addProfileButton->setText(tr("Add"));
    }

    StationProfile profile;

    profile.profileName = ui->profileEdit->text();
    profile.callsign = ui->callsignEdit->text().toUpper();
    profile.locator = ui->locatorEdit->text().toUpper();
    profile.operatorName = ui->operatorEdit->text();
    profile.qthName = ui->qthEdit->text();
    profile.iota = ui->iotaEdit->text().toUpper();
    profile.sota = ui->sotaEdit->text().toUpper();
    profile.sig = ui->sigEdit->text().toUpper();
    profile.sigInfo = ui->sigInfoEdit->text();
    profile.vucc = ui->vuccEdit->text().toUpper();

    profileManager->add(profile);
    refreshStationProfilesView();

    ui->profileEdit->clear();
    ui->profileEdit->setPlaceholderText(QString());
    ui->callsignEdit->clear();
    ui->callsignEdit->setPlaceholderText(QString());
    ui->locatorEdit->clear();
    ui->locatorEdit->setPlaceholderText(QString());
    ui->operatorEdit->clear();
    ui->qthEdit->clear();
    ui->sotaEdit->clear();
    ui->iotaEdit->clear();
    ui->sigEdit->clear();
    ui->sigInfoEdit->clear();
    ui->vuccEdit->clear();
}

void SettingsDialog::deleteStationProfile()
{
    FCT_IDENTIFICATION;

    foreach (QModelIndex index, ui->profilesListView->selectionModel()->selectedRows()) {
        profileManager->remove(ui->profilesListView->model()->data(index).toString());
        ui->profilesListView->model()->removeRow(index.row());
    }
    ui->profilesListView->clearSelection();
}

void SettingsDialog::doubleClickStationProfile(QModelIndex i)
{
    FCT_IDENTIFICATION;

    StationProfile profile;

    profile = profileManager->get(ui->profilesListView->model()->data(i).toString());

    ui->profileEdit->setText(profile.profileName);
    ui->callsignEdit->setText(profile.callsign);
    ui->locatorEdit->setText(profile.locator);
    ui->operatorEdit->setText(profile.operatorName);
    ui->qthEdit->setText(profile.qthName);
    ui->iotaEdit->setText(profile.iota);
    ui->sotaEdit->setText(profile.sota);
    ui->sigEdit->setText(profile.sig);
    ui->sigInfoEdit->setText(profile.sigInfo);
    ui->vuccEdit->setText(profile.vucc);

    ui->addProfileButton->setText(tr("Modify"));
}

void SettingsDialog::rigChanged(int index)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<index;

    const struct rig_caps *caps;

    QModelIndex rig_index = ui->rigModelSelect->model()->index(index, 0);
    caps = rig_get_caps(rig_index.internalId());

    if ( caps )
    {
        if ( caps->port_type == RIG_PORT_NETWORK
             || caps->port_type == RIG_PORT_UDP_NETWORK)
        {
            ui->rigStackedWidget->setCurrentIndex(1);
        }
        else
        {
            ui->rigStackedWidget->setCurrentIndex(0);
        }
    }
    else
    {
        ui->rigStackedWidget->setCurrentIndex(0);
    }
}

void SettingsDialog::rotChanged(int index)
{
    FCT_IDENTIFICATION;

    const struct rot_caps *caps;

    QModelIndex rot_index = ui->rotModelSelect->model()->index(index, 0);
    caps = rot_get_caps(rot_index.internalId());

    if ( caps )
    {
        if ( caps->port_type == RIG_PORT_NETWORK
             || caps->port_type == RIG_PORT_UDP_NETWORK)
        {
            ui->rotStackedWidget->setCurrentIndex(1);
        }
        else
        {
            ui->rotStackedWidget->setCurrentIndex(0);
        }
    }
    else
    {
        ui->rotStackedWidget->setCurrentIndex(0);
    }

}

void SettingsDialog::tqslPathBrowse()
{
    FCT_IDENTIFICATION;

    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Select File"),
                                                    "",
#if defined(Q_OS_WIN)
                                                    "TQSL (*.exe)"
#elif (Q_OS_MACOS)
                                                    "TQSL (*.app)"
#else
                                                    "TQSL (tqsl)"
#endif
                                                   );
    if ( !filename.isEmpty() )
    {
        ui->tqslPathEdit->setText(filename);
    }
}

void SettingsDialog::adjustCallsignTextColor()
{
    FCT_IDENTIFICATION;

    if ( ! ui->callsignEdit->hasAcceptableInput() )
    {
        ui->callsignEdit->setStyleSheet("QLineEdit { color: red;}");
    }
    else
    {
        ui->callsignEdit->setStyleSheet("QLineEdit { color: black;}");
    }

}

void SettingsDialog::adjustLocatorTextColor()
{
    FCT_IDENTIFICATION;

    if ( ! ui->locatorEdit->hasAcceptableInput() )
    {
        ui->locatorEdit->setStyleSheet("QLineEdit { color: red;}");
    }
    else
    {
        ui->locatorEdit->setStyleSheet("QLineEdit { color: black;}");
    }

}

void SettingsDialog::adjustVUCCLocatorTextColor()
{
    FCT_IDENTIFICATION;

    if ( ! ui->vuccEdit->hasAcceptableInput() )
    {
        ui->vuccEdit->setStyleSheet("QLineEdit { color: red;}");
    }
    else
    {
        ui->vuccEdit->setStyleSheet("QLineEdit { color: black;}");
    }
}

void SettingsDialog::eqslDirBrowse()
{
    FCT_IDENTIFICATION;

    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select Directory"),
#if defined (Q_OS_WIN)
                                                    "C:\\",
#else
                                                    "~",
#endif
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if ( !dir.isEmpty() )
    {
        ui->eqslFolderPathEdit->setText(dir);
    }
}

void SettingsDialog::cancelled()
{
    FCT_IDENTIFICATION;

    if ( profileManager->profilesList().isEmpty() )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                             QMessageBox::tr("Please, define at least one Station Locations Profile"));
        return;
    }

    reject();
}

void SettingsDialog::sotaChanged(QString newSOTA)
{
    FCT_IDENTIFICATION;

    if ( newSOTA.length() >= 3 )
    {
        ui->sotaEdit->setCompleter(sotaCompleter);
    }
    else
    {
        ui->sotaEdit->setCompleter(nullptr);
    }
}

void SettingsDialog::readSettings() {
    FCT_IDENTIFICATION;

    QSettings settings;
    QString username;

    QStringList profiles = profileManager->profilesList();
    ((QStringListModel*)ui->profilesListView->model())->setStringList(profiles);

    QStringList rigs = settings.value("station/rigs").toStringList();
    ((QStringListModel*)ui->rigListView->model())->setStringList(rigs);

    QStringList ants = settings.value("station/antennas").toStringList();
    ((QStringListModel*)ui->antListView->model())->setStringList(ants);

    ui->rigModelSelect->setCurrentIndex(settings.value("hamlib/rig/modelrow").toInt());
    ui->rigPortEdit->setText(settings.value("hamlib/rig/port").toString());
    ui->rigBaudSelect->setCurrentText(settings.value("hamlib/rig/baudrate").toString());
    ui->rigDataBitsSelect->setCurrentText(settings.value("hamlib/rig/databits").toString());
    ui->rigStopBitsSelect->setCurrentText(settings.value("hamlib/rig/stopbits").toString());
    ui->rigFlowControlSelect->setCurrentText(settings.value("hamlib/rig/flowcontrol").toString());
    ui->rigParitySelect->setCurrentText(settings.value("hamlib/rig/parity").toString());
    ui->rigHostNameEdit->setText(settings.value("hamlib/rig/hostname").toString());
    ui->rigNetPortSpin->setValue(settings.value("hamlib/rig/netport").toInt());

    ui->rotModelSelect->setCurrentIndex(settings.value("hamlib/rot/modelrow").toInt());
    ui->rotPortEdit->setText(settings.value("hamlib/rot/port").toString());
    ui->rotBaudSelect->setCurrentText(settings.value("hamlib/rot/baudrate").toString());
    ui->rotDataBitsSelect->setCurrentText(settings.value("hamlib/rot/databits").toString());
    ui->rotStopBitsSelect->setCurrentText(settings.value("hamlib/rot/stopbits").toString());
    ui->rotFlowControlSelect->setCurrentText(settings.value("hamlib/rot/flowcontrol").toString());
    ui->rotParitySelect->setCurrentText(settings.value("hamlib/rot/parity").toString());
    ui->rotHostNameEdit->setText(settings.value("hamlib/rot/hostname").toString());
    ui->rotNetPortSpin->setValue(settings.value("hamlib/rot/netport").toInt());

    /************/
    /* Callbook */
    /************/
    ui->callbookCombo->setCurrentText(settings.value(GenericCallbook::CONFIG_SELECTED_CALLBOOK_KEY,"HamQTH").toString());
    username = settings.value(GenericCallbook::CONFIG_USERNAME_KEY).toString();
    ui->callbookUsernameEdit->setText(username);
    ui->callbookPasswordEdit->setText(CredentialStore::instance()->getPassword(GenericCallbook::SECURE_STORAGE_KEY,
                                                                               username));
    /********/
    /* LoTW */
    /********/
    username = settings.value(Lotw::CONFIG_USERNAME_KEY).toString();
    ui->lotwUsernameEdit->setText(username);
    ui->lotwPasswordEdit->setText(CredentialStore::instance()->getPassword(Lotw::SECURE_STORAGE_KEY,
                                                                           username));
    ui->tqslPathEdit->setText(settings.value("lotw/tqsl").toString());

    /***********/
    /* ClubLog */
    /***********/
    username = settings.value(ClubLog::CONFIG_EMAIL_KEY).toString();
    ui->clublogEmailEdit->setText(username);
    ui->clublogCallsignEdit->setText(settings.value(ClubLog::CONFIG_CALLSIGN_KEY).toString());
    ui->clublogPasswordEdit->setText(CredentialStore::instance()->getPassword(ClubLog::SECURE_STORAGE_KEY,
                                                                              username));

    /********/
    /* eQSL */
    /********/
    username = settings.value(EQSL::CONFIG_USERNAME_KEY).toString();
    ui->eqslUsernameEdit->setText(username);
    ui->eqslPasswordEdit->setText(CredentialStore::instance()->getPassword(EQSL::SECURE_STORAGE_KEY,
                                                                           username));
    ui->eqslFolderPathEdit->setText(settings.value(EQSL::CONFIG_QSL_FOLDER_KEY,QStandardPaths::writableLocation(QStandardPaths::DataLocation)).toString());

    if (!settings.value("dxcc/start").isNull()) {
       ui->dxccStartDateCheckBox->setCheckState(Qt::Checked);
       ui->dxccStartDate->setDate(settings.value("dxcc/start").toDate());
    }
    else {
        ui->dxccStartDateCheckBox->setCheckState(Qt::Unchecked);
        ui->dxccStartDate->setDate(QDate::currentDate());
    }

    //hamlib has hardcoded port number. Therefore we disable the SpinBox
    //until hamlib guyes fix it.
    ui->rigNetPortSpin->setDisabled(true);
    ui->rotNetPortSpin->setDisabled(true);
}

void SettingsDialog::writeSettings() {
    FCT_IDENTIFICATION;

    QSettings settings;
    QString old_username;

    profileManager->save();

    QStringList rigs = ((QStringListModel*)ui->rigListView->model())->stringList();
    settings.setValue("station/rigs", rigs);

    QStringList ants = ((QStringListModel*)ui->antListView->model())->stringList();
    settings.setValue("station/antennas", ants);

    int rig_row = ui->rigModelSelect->currentIndex();
    QModelIndex rig_index = ui->rigModelSelect->model()->index(rig_row, 0);
    settings.setValue("hamlib/rig/model", rig_index.internalId());
    settings.setValue("hamlib/rig/modelrow", rig_row);
    settings.setValue("hamlib/rig/port", ui->rigPortEdit->text());
    settings.setValue("hamlib/rig/baudrate", ui->rigBaudSelect->currentText());
    settings.setValue("hamlib/rig/databits", ui->rigDataBitsSelect->currentText());
    settings.setValue("hamlib/rig/stopbits", ui->rigStopBitsSelect->currentText());
    settings.setValue("hamlib/rig/flowcontrol", ui->rigFlowControlSelect->currentText());
    settings.setValue("hamlib/rig/parity", ui->rigParitySelect->currentText());
    settings.setValue("hamlib/rig/hostname", ui->rigHostNameEdit->text());
    settings.setValue("hamlib/rig/netport", ui->rigNetPortSpin->value());

    int rot_row = ui->rotModelSelect->currentIndex();
    QModelIndex rot_index = ui->rotModelSelect->model()->index(rot_row, 0);
    settings.setValue("hamlib/rot/model", rot_index.internalId());
    settings.setValue("hamlib/rot/modelrow", rot_row);
    settings.setValue("hamlib/rot/port", ui->rotPortEdit->text());
    settings.setValue("hamlib/rot/baudrate", ui->rotBaudSelect->currentText());
    settings.setValue("hamlib/rot/databits", ui->rotDataBitsSelect->currentText());
    settings.setValue("hamlib/rot/stopbits", ui->rotStopBitsSelect->currentText());
    settings.setValue("hamlib/rot/flowcontrol", ui->rotFlowControlSelect->currentText());
    settings.setValue("hamlib/rot/parity", ui->rotParitySelect->currentText());
    settings.setValue("hamlib/rot/hostname", ui->rotHostNameEdit->text());
    settings.setValue("hamlib/rot/netport", ui->rotNetPortSpin->value());

    /************/
    /* Callbook */
    /************/
    old_username = settings.value(GenericCallbook::CONFIG_USERNAME_KEY).toString();
    if ( old_username != ui->callbookUsernameEdit->text() )
    {
        CredentialStore::instance()->deletePassword(GenericCallbook::SECURE_STORAGE_KEY,
                                                    old_username);
    }

    settings.setValue(GenericCallbook::CONFIG_USERNAME_KEY, ui->callbookUsernameEdit->text());

    CredentialStore::instance()->savePassword(GenericCallbook::SECURE_STORAGE_KEY,
                                              ui->callbookUsernameEdit->text(),
                                              ui->callbookPasswordEdit->text());

    settings.setValue(GenericCallbook::CONFIG_SELECTED_CALLBOOK_KEY, ui->callbookCombo->currentText());

    /********/
    /* LoTW */
    /********/
    old_username = settings.value(Lotw::CONFIG_USERNAME_KEY).toString();
    if ( old_username != ui->lotwUsernameEdit->text() )
    {
        CredentialStore::instance()->deletePassword(Lotw::SECURE_STORAGE_KEY,
                                                    old_username);
    }
    settings.setValue(Lotw::CONFIG_USERNAME_KEY, ui->lotwUsernameEdit->text());
    CredentialStore::instance()->savePassword(Lotw::SECURE_STORAGE_KEY,
                                              ui->lotwUsernameEdit->text(),
                                              ui->lotwPasswordEdit->text());
    settings.setValue("lotw/tqsl", ui->tqslPathEdit->text());

    /***********/
    /* ClubLog */
    /***********/
    old_username = settings.value(ClubLog::CONFIG_EMAIL_KEY).toString();
    if ( old_username != ui->clublogEmailEdit->text() )
    {
        CredentialStore::instance()->deletePassword(ClubLog::SECURE_STORAGE_KEY,
                                                    old_username);
    }
    settings.setValue(ClubLog::CONFIG_EMAIL_KEY, ui->clublogEmailEdit->text());
    settings.setValue(ClubLog::CONFIG_CALLSIGN_KEY, ui->clublogCallsignEdit->text());
    CredentialStore::instance()->savePassword(ClubLog::SECURE_STORAGE_KEY,
                                              ui->clublogEmailEdit->text(),
                                              ui->clublogPasswordEdit->text());

    /********/
    /* eQSL */
    /********/
    old_username = settings.value(EQSL::CONFIG_USERNAME_KEY).toString();
    if ( old_username != ui->eqslUsernameEdit->text() )
    {
        CredentialStore::instance()->deletePassword(EQSL::SECURE_STORAGE_KEY,
                                                    old_username);
    }
    settings.setValue(EQSL::CONFIG_USERNAME_KEY, ui->eqslUsernameEdit->text());
    CredentialStore::instance()->savePassword(EQSL::SECURE_STORAGE_KEY,
                                              ui->eqslUsernameEdit->text(),
                                              ui->eqslPasswordEdit->text());

    settings.setValue(EQSL::CONFIG_QSL_FOLDER_KEY, ui->eqslFolderPathEdit->text());

    if (ui->dxccStartDateCheckBox->isChecked()) {
        settings.setValue("dxcc/start", ui->dxccStartDate->date());
    }
    else {
        settings.setValue("dxcc/start", QVariant());
    }
}

SettingsDialog::~SettingsDialog() {
    FCT_IDENTIFICATION;

    modeTableModel->deleteLater();
    bandTableModel->deleteLater();
    sotaCompleter->deleteLater();
    iotaCompleter->deleteLater();
    delete ui;
}
