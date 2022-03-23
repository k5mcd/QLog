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
#include "../core/QRZ.h"
#include "../core/HamQTH.h"
#include "../core/Lotw.h"
#include "../core/ClubLog.h"
#include "../core/Eqsl.h"
#include "../core/StyleItemDelegate.h"
#include "core/debug.h"
#include "core/CredentialStore.h"
#include "data/StationProfile.h"
#include "data/RigProfile.h"
#include "data/Data.h"
#include "core/Gridsquare.h"
#include "core/Wsjtx.h"
#include "core/PaperQSL.h"
#include "core/NetworkNotification.h"

#define WIDGET_INDEX_SERIAL_RIG  0
#define STACKED_WIDGET_NETWORK_RIG 1


MODULE_IDENTIFICATION("qlog.ui.settingdialog");


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    stationProfManager(StationProfilesManager::instance()),
    rigProfManager(RigProfilesManager::instance()),
    ui(new Ui::SettingsDialog)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    RigTypeModel* rigTypeModel = new RigTypeModel(this);
    ui->rigModelSelect->setModel(rigTypeModel);

    RotTypeModel* rotTypeModel = new RotTypeModel(this);
    ui->rotModelSelect->setModel(rotTypeModel);

    QStringListModel* rigModel = new QStringListModel();
    ui->rigProfilesListView->setModel(rigModel);

    QStringListModel* antModel = new QStringListModel();
    ui->antProfilesListView->setModel(antModel);

    QStringListModel* profilesModes = new QStringListModel();
    ui->stationProfilesListView->setModel(profilesModes);

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

    ui->stationCallsignEdit->setValidator(new QRegularExpressionValidator(Data::callsignRegEx(), this));
    ui->stationLocatorEdit->setValidator(new QRegularExpressionValidator(Gridsquare::gridRegEx(), this));
    ui->stationVUCCEdit->setValidator(new QRegularExpressionValidator(Gridsquare::gridVUCCRegEx(), this));

    iotaCompleter = new QCompleter(Data::instance()->iotaIDList(), this);
    iotaCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    iotaCompleter->setFilterMode(Qt::MatchContains);
    iotaCompleter->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    ui->stationIOTAEdit->setCompleter(iotaCompleter);

    sotaCompleter = new QCompleter(Data::instance()->sotaIDList(), this);
    sotaCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    sotaCompleter->setFilterMode(Qt::MatchStartsWith);
    sotaCompleter->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    ui->stationSOTAEdit->setCompleter(nullptr);

    ui->primaryCallbookCombo->addItem(tr("Disabled"), QVariant(GenericCallbook::CALLBOOK_NAME));
    ui->primaryCallbookCombo->addItem(tr("HamQTH"),   QVariant(HamQTH::CALLBOOK_NAME));
    ui->primaryCallbookCombo->addItem(tr("QRZ.com"),  QVariant(QRZ::CALLBOOK_NAME));

    ui->rigModelSelect->setCurrentIndex(ui->rigModelSelect->findData(DEFAULT_RIG_MODEL));

    readSettings();
}

void SettingsDialog::save() {
    FCT_IDENTIFICATION;

    if ( stationProfManager->profileNameList().isEmpty() )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                             QMessageBox::tr("Please, define at least one Station Locations Profile"));
        return;
    }

    writeSettings();
    accept();
}

void SettingsDialog::addRigProfile()
{
    FCT_IDENTIFICATION;

    if ( ui->rigProfileNameEdit->text().isEmpty() )
    {
        ui->rigProfileNameEdit->setPlaceholderText(tr("Must not be empty"));
        return;
    }

    if ( ui->rigAddProfileButton->text() == tr("Modify"))
    {
        ui->rigAddProfileButton->setText(tr("Add"));
    }

    RigProfile profile;


    profile.profileName = ui->rigProfileNameEdit->text();

    profile.model = ui->rigModelSelect->currentData().toInt();

    profile.hostname = ( ui->rigStackedWidget->currentIndex() == WIDGET_INDEX_SERIAL_RIG ) ?
                QString() :
                ui->rigHostNameEdit->text();

    profile.netport = ( ui->rigStackedWidget->currentIndex() == WIDGET_INDEX_SERIAL_RIG ) ?
                0 :
                ui->rigNetPortSpin->value();

    profile.portPath = ( ui->rigStackedWidget->currentIndex() == WIDGET_INDEX_SERIAL_RIG ) ?
                ui->rigPortEdit->text() :
                QString();

    profile.baudrate = ( ui->rigStackedWidget->currentIndex() == WIDGET_INDEX_SERIAL_RIG ) ?
                ui->rigBaudSelect->currentText().toInt() :
                0;

    profile.databits = ( ui->rigStackedWidget->currentIndex() == WIDGET_INDEX_SERIAL_RIG ) ?
                ui->rigDataBitsSelect->currentText().toInt():
                0;

    profile.stopbits = ( ui->rigStackedWidget->currentIndex() == WIDGET_INDEX_SERIAL_RIG ) ?
                ui->rigStopBitsSelect->currentText().toFloat() :
                0;

    profile.flowcontrol = ( ui->rigStackedWidget->currentIndex() == WIDGET_INDEX_SERIAL_RIG ) ?
                ui->rigFlowControlSelect->currentText() :
                0;

    profile.parity = ( ui->rigStackedWidget->currentIndex() == WIDGET_INDEX_SERIAL_RIG ) ?
                ui->rigParitySelect->currentText():
                QString();

    rigProfManager->addProfile(profile.profileName, profile);

    refreshRigProfilesView();

    ui->rigProfileNameEdit->setPlaceholderText(QString());
    ui->rigPortEdit->setPlaceholderText(QString());
    ui->rigHostNameEdit->setPlaceholderText(QString());

    ui->rigProfileNameEdit->clear();
    ui->rigModelSelect->setCurrentIndex(ui->rigModelSelect->findData(DEFAULT_RIG_MODEL));
    ui->rigPortEdit->clear();
    ui->rigHostNameEdit->clear();
    ui->rigBaudSelect->setCurrentIndex(0);
    ui->rigDataBitsSelect->setCurrentIndex(0);
    ui->rigStopBitsSelect->setCurrentIndex(0);
    ui->rigFlowControlSelect->setCurrentIndex(0);
    ui->rigParitySelect->setCurrentIndex(0);
}

void SettingsDialog::delRigProfile()
{
    FCT_IDENTIFICATION;

    foreach (QModelIndex index, ui->rigProfilesListView->selectionModel()->selectedRows())
    {
        rigProfManager->removeProfile(ui->rigProfilesListView->model()->data(index).toString());
        ui->rigProfilesListView->model()->removeRow(index.row());
    }
    ui->rigProfilesListView->clearSelection();
}

void SettingsDialog::doubleClickRigProfile(QModelIndex i)
{
    FCT_IDENTIFICATION;

    RigProfile profile;

    profile = rigProfManager->getProfile(ui->rigProfilesListView->model()->data(i).toString());

    ui->rigProfileNameEdit->setText(profile.profileName);

    ui->rigModelSelect->setCurrentIndex(ui->rigModelSelect->findData(profile.model));
    ui->rigPortEdit->setText(profile.portPath);
    ui->rigHostNameEdit->setText(profile.hostname);
    ui->rigNetPortSpin->setValue(profile.netport);
    ui->rigBaudSelect->setCurrentText(QString::number(profile.baudrate));
    ui->rigDataBitsSelect->setCurrentText(QString::number(profile.databits));
    ui->rigStopBitsSelect->setCurrentText(QString::number(profile.stopbits));
    ui->rigFlowControlSelect->setCurrentText(profile.flowcontrol);
    ui->rigParitySelect->setCurrentText(profile.parity);

    ui->rigAddProfileButton->setText(tr("Modify"));
}

void SettingsDialog::addAnt() {
    FCT_IDENTIFICATION;
/*
    if (ui->antennasEdit->text().isEmpty()) return;

    QStringListModel* model = (QStringListModel*)ui->antListView->model();
    QStringList ants = model->stringList();
    ants << ui->antennasEdit->text();
    model->setStringList(ants);
    ui->antennasEdit->clear();
    */
}

void SettingsDialog::deleteAnt() {
    FCT_IDENTIFICATION;
/*
    foreach (QModelIndex index, ui->antListView->selectionModel()->selectedRows()) {
        ui->antListView->model()->removeRow(index.row());
    }
    ui->antListView->clearSelection();
    */
}

void SettingsDialog::refreshRigProfilesView()
{
    FCT_IDENTIFICATION;
    QStringListModel* model = (QStringListModel*)ui->rigProfilesListView->model();
    QStringList profiles = model->stringList();

    profiles.clear();

    profiles << rigProfManager->profileNameList();

    model->setStringList(profiles);
}

void SettingsDialog::refreshStationProfilesView()
{
    FCT_IDENTIFICATION;
    QStringListModel* model = (QStringListModel*)ui->stationProfilesListView->model();
    QStringList profiles = model->stringList();

    profiles.clear();

    profiles << stationProfManager->profileNameList();

    model->setStringList(profiles);
}

void SettingsDialog::addStationProfile()
{
    FCT_IDENTIFICATION;

    if ( ui->stationProfileNameEdit->text().isEmpty() )
    {
        ui->stationProfileNameEdit->setPlaceholderText(tr("Must not be empty"));
        return;
    }

    if ( ui->stationCallsignEdit->text().isEmpty() )
    {
        ui->stationCallsignEdit->setPlaceholderText(tr("Must not be empty"));
        return;
    }

    if ( ui->stationLocatorEdit->text().isEmpty() )
    {
        ui->stationLocatorEdit->setPlaceholderText(tr("Must not be empty"));
        return;
    }

    if ( ! ui->stationCallsignEdit->hasAcceptableInput() )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                             QMessageBox::tr("Callsign has an invalid format"));
        return;
    }

    if ( ! ui->stationLocatorEdit->hasAcceptableInput() )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                             QMessageBox::tr("Locator has an invalid format"));
        return;
    }

    if ( ! ui->stationVUCCEdit->text().isEmpty() )
    {
        if ( ! ui->stationVUCCEdit->hasAcceptableInput() )
        {
            QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                                 QMessageBox::tr("VUCC Locator has an invalid format (must be 2 or 4 locators separated by ',')"));
            return;
        }
    }

    if ( ui->stationAddProfileButton->text() == tr("Modify"))
    {
        ui->stationAddProfileButton->setText(tr("Add"));
    }

    StationProfile profile;

    profile.profileName = ui->stationProfileNameEdit->text();
    profile.callsign = ui->stationCallsignEdit->text().toUpper();
    profile.locator = ui->stationLocatorEdit->text().toUpper();
    profile.operatorName = ui->stationOperatorEdit->text();
    profile.qthName = ui->stationQTHEdit->text();
    profile.iota = ui->stationIOTAEdit->text().toUpper();
    profile.sota = ui->stationSOTAEdit->text().toUpper();
    profile.sig = ui->stationSIGEdit->text().toUpper();
    profile.sigInfo = ui->stationSIGInfoEdit->text();
    profile.vucc = ui->stationVUCCEdit->text().toUpper();

    stationProfManager->addProfile(profile.profileName, profile);

    refreshStationProfilesView();

    ui->stationProfileNameEdit->clear();
    ui->stationProfileNameEdit->setPlaceholderText(QString());
    ui->stationCallsignEdit->clear();
    ui->stationCallsignEdit->setPlaceholderText(QString());
    ui->stationLocatorEdit->clear();
    ui->stationLocatorEdit->setPlaceholderText(QString());
    ui->stationOperatorEdit->clear();
    ui->stationQTHEdit->clear();
    ui->stationSOTAEdit->clear();
    ui->stationIOTAEdit->clear();
    ui->stationSIGEdit->clear();
    ui->stationSIGInfoEdit->clear();
    ui->stationVUCCEdit->clear();
}

void SettingsDialog::deleteStationProfile()
{
    FCT_IDENTIFICATION;

    foreach (QModelIndex index, ui->stationProfilesListView->selectionModel()->selectedRows()) {
        stationProfManager->removeProfile(ui->stationProfilesListView->model()->data(index).toString());
        ui->stationProfilesListView->model()->removeRow(index.row());
    }
    ui->stationProfilesListView->clearSelection();
}

void SettingsDialog::doubleClickStationProfile(QModelIndex i)
{
    FCT_IDENTIFICATION;

    StationProfile profile;

    profile = stationProfManager->getProfile(ui->stationProfilesListView->model()->data(i).toString());

    ui->stationProfileNameEdit->setText(profile.profileName);
    ui->stationCallsignEdit->setText(profile.callsign);
    ui->stationLocatorEdit->setText(profile.locator);
    ui->stationOperatorEdit->setText(profile.operatorName);
    ui->stationQTHEdit->setText(profile.qthName);
    ui->stationIOTAEdit->setText(profile.iota);
    ui->stationSOTAEdit->setText(profile.sota);
    ui->stationSIGEdit->setText(profile.sig);
    ui->stationSIGInfoEdit->setText(profile.sigInfo);
    ui->stationVUCCEdit->setText(profile.vucc);

    ui->stationAddProfileButton->setText(tr("Modify"));
}

void SettingsDialog::rigChanged(int index)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<index;

    const struct rig_caps *caps;

    int rigID = ui->rigModelSelect->currentData().toInt();

    caps = rig_get_caps(rigID);

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

    QPalette p;

    if ( ! ui->stationCallsignEdit->hasAcceptableInput() )
    {
        p.setColor(QPalette::Text,Qt::red);
    }
    else
    {
        p.setColor(QPalette::Text,qApp->palette().text().color());
    }
    ui->stationCallsignEdit->setPalette(p);

}

void SettingsDialog::adjustLocatorTextColor()
{
    FCT_IDENTIFICATION;

    QPalette p;

    if ( ! ui->stationLocatorEdit->hasAcceptableInput() )
    {
        p.setColor(QPalette::Text,Qt::red);
    }
    else
    {
        p.setColor(QPalette::Text,qApp->palette().text().color());
    }

    ui->stationLocatorEdit->setPalette(p);

}

void SettingsDialog::adjustVUCCLocatorTextColor()
{
    FCT_IDENTIFICATION;

    QPalette p;

    if ( ! ui->stationVUCCEdit->hasAcceptableInput() )
    {
        p.setColor(QPalette::Text,Qt::red);
    }
    else
    {
        p.setColor(QPalette::Text,qApp->palette().text().color());
    }

    ui->stationVUCCEdit->setPalette(p);
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

void SettingsDialog::paperDirBrowse()
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
        ui->paperFolderPathEdit->setText(dir);
    }
}

void SettingsDialog::cancelled()
{
    FCT_IDENTIFICATION;

    if ( stationProfManager->profileNameList().isEmpty() )
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
        ui->stationSOTAEdit->setCompleter(sotaCompleter);
    }
    else
    {
        ui->stationSOTAEdit->setCompleter(nullptr);
    }
}

void SettingsDialog::primaryCallbookChanged(int index)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << index;

    QString primaryCallbookSelection = ui->primaryCallbookCombo->itemData(index).toString();

    if ( primaryCallbookSelection == GenericCallbook::CALLBOOK_NAME )
    {
        ui->secondaryCallbookCombo->clear();
        ui->secondaryCallbookCombo->setEnabled(false);
    }
    else if ( primaryCallbookSelection == HamQTH::CALLBOOK_NAME )
    {
        ui->secondaryCallbookCombo->setEnabled(true);
        ui->secondaryCallbookCombo->clear();
        ui->secondaryCallbookCombo->addItem(tr("Disabled"), QVariant(GenericCallbook::CALLBOOK_NAME));
        ui->secondaryCallbookCombo->addItem(tr("QRZ.com"),  QVariant(QRZ::CALLBOOK_NAME));
    }
    else if ( primaryCallbookSelection == QRZ::CALLBOOK_NAME )
    {
        ui->secondaryCallbookCombo->setEnabled(true);
        ui->secondaryCallbookCombo->clear();
        ui->secondaryCallbookCombo->addItem(tr("Disabled"), QVariant(GenericCallbook::CALLBOOK_NAME));
        ui->secondaryCallbookCombo->addItem(tr("HamQTH"),  QVariant(HamQTH::CALLBOOK_NAME));
    }
}

void SettingsDialog::secondaryCallbookChanged(int index)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << index;
}

void SettingsDialog::readSettings() {
    FCT_IDENTIFICATION;

    QSettings settings;

    QStringList profiles = stationProfManager->profileNameList();
    ((QStringListModel*)ui->stationProfilesListView->model())->setStringList(profiles);

    QStringList rigs = rigProfManager->profileNameList();
    ((QStringListModel*)ui->rigProfilesListView->model())->setStringList(rigs);

    QStringList ants = settings.value("station/antennas").toStringList();
    ((QStringListModel*)ui->antProfilesListView->model())->setStringList(ants);

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

    int primaryCallbookIndex = ui->primaryCallbookCombo->findData(settings.value(GenericCallbook::CONFIG_PRIMARY_CALLBOOK_KEY,
                                                                                 GenericCallbook::CALLBOOK_NAME));

    ui->primaryCallbookCombo->setCurrentIndex(primaryCallbookIndex);

    int secondaryCallbookIndex = ui->secondaryCallbookCombo->findData(settings.value(GenericCallbook::CONFIG_SECONDARY_CALLBOOK_KEY,
                                                                                     GenericCallbook::CALLBOOK_NAME));

    ui->secondaryCallbookCombo->setCurrentIndex(secondaryCallbookIndex);

    ui->hamQthUsernameEdit->setText(HamQTH::getUsername());
    ui->hamQthPasswordEdit->setText(HamQTH::getPassword());

    ui->qrzUsernameEdit->setText(QRZ::getUsername());
    ui->qrzPasswordEdit->setText(QRZ::getPassword());

    /********/
    /* LoTW */
    /********/
    ui->lotwUsernameEdit->setText(Lotw::getUsername());
    ui->lotwPasswordEdit->setText(Lotw::getPassword());
    ui->tqslPathEdit->setText(Lotw::getTQSLPath());

    /***********/
    /* ClubLog */
    /***********/
    ui->clublogEmailEdit->setText(ClubLog::getEmail());
    ui->clublogCallsignEdit->setText(ClubLog::getRegisteredCallsign());
    ui->clublogPasswordEdit->setText(ClubLog::getPassword());

    /********/
    /* eQSL */
    /********/
    ui->eqslUsernameEdit->setText(EQSL::getUsername());
    ui->eqslPasswordEdit->setText(EQSL::getPassword());
    ui->eqslFolderPathEdit->setText(EQSL::getQSLImageFolder());

    /***********/
    /* QRZ.COM */
    /***********/
    ui->qrzApiKeyEdit->setText(QRZ::getLogbookAPIKey());

    /*************/
    /* Paper QSL */
    /*************/
    ui->paperFolderPathEdit->setText(PaperQSL::getQSLImageFolder());

    /********/
    /* DXCC */
    /********/
    if (!settings.value("dxcc/start").isNull()) {
       ui->dxccStartDateCheckBox->setCheckState(Qt::Checked);
       ui->dxccStartDate->setDate(settings.value("dxcc/start").toDate());
    }
    else {
        ui->dxccStartDateCheckBox->setCheckState(Qt::Unchecked);
        ui->dxccStartDate->setDate(QDate::currentDate());
    }

    /***********/
    /* NETWORK */
    /***********/

    ui->wsjtPortSpin->setValue(Wsjtx::getConfigPort());
    ui->wsjtForwardEdit->setText(Wsjtx::getConfigForwardAddresses());

    ui->notifQSOEdit->setText(NetworkNotification::getNotifQSOAdiAddrs());
    ui->notifDXSpotsEdit->setText(NetworkNotification::getNotifDXSpotAddrs());
    ui->notifWSJTXCQSpotsEdit->setText(NetworkNotification::getNotifWSJTXCQSpotAddrs());

    /******************/
    /* END OF Reading */
    /******************/

    //hamlib has hardcoded port number. Therefore we disable the SpinBox
    //until hamlib guyes fix it.
    ui->rigNetPortSpin->setDisabled(true);
    ui->rotNetPortSpin->setDisabled(true);


}

void SettingsDialog::writeSettings() {
    FCT_IDENTIFICATION;

    QSettings settings;

    stationProfManager->save();
    rigProfManager->save();

    QStringList ants = ((QStringListModel*)ui->antProfilesListView->model())->stringList();
    settings.setValue("station/antennas", ants);

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
    HamQTH::saveUsernamePassword(ui->hamQthUsernameEdit->text(),
                                 ui->hamQthPasswordEdit->text());

    QRZ::saveUsernamePassword(ui->qrzUsernameEdit->text(),
                              ui->qrzPasswordEdit->text());

    settings.setValue(GenericCallbook::CONFIG_PRIMARY_CALLBOOK_KEY,
                      ui->primaryCallbookCombo->itemData(ui->primaryCallbookCombo->currentIndex()).toString());
    settings.setValue(GenericCallbook::CONFIG_SECONDARY_CALLBOOK_KEY,
                      ui->secondaryCallbookCombo->itemData(ui->secondaryCallbookCombo->currentIndex()).toString());

    /********/
    /* LoTW */
    /********/

    Lotw::saveUsernamePassword(ui->lotwUsernameEdit->text(),
                               ui->lotwPasswordEdit->text());
    Lotw::saveTQSLPath(ui->tqslPathEdit->text());

    /***********/
    /* ClubLog */
    /***********/
    ClubLog::saveRegistredCallsign(ui->clublogCallsignEdit->text());
    ClubLog::saveUsernamePassword(ui->clublogEmailEdit->text(),
                                  ui->clublogPasswordEdit->text());

    /********/
    /* eQSL */
    /********/

    EQSL::saveUsernamePassword(ui->eqslUsernameEdit->text(),
                               ui->eqslPasswordEdit->text());
    EQSL::saveQSLImageFolder(ui->eqslFolderPathEdit->text());

    /***********/
    /* QRZ.COM */
    /***********/
    QRZ::saveLogbookAPI(ui->qrzApiKeyEdit->text());


    /*********/
    /* DXCC  */
    /*********/
    if (ui->dxccStartDateCheckBox->isChecked()) {
        settings.setValue("dxcc/start", ui->dxccStartDate->date());
    }
    else {
        settings.setValue("dxcc/start", QVariant());
    }

    /*************/
    /* Paper QSL */
    /*************/
    PaperQSL::saveQSLImageFolder(ui->paperFolderPathEdit->text());

    /***********/
    /* NETWORK */
    /***********/
    Wsjtx::saveConfigPort(ui->wsjtPortSpin->value());
    Wsjtx::saveConfigForwardAddresses(ui->wsjtForwardEdit->text());

    NetworkNotification::saveNotifQSOAdiAddrs(ui->notifQSOEdit->text());
    NetworkNotification::saveNotifDXSpotAddrs(ui->notifDXSpotsEdit->text());
    NetworkNotification::saveNotifWSJTXCQSpotAddrs(ui->notifWSJTXCQSpotsEdit->text());
}

SettingsDialog::~SettingsDialog() {
    FCT_IDENTIFICATION;

    modeTableModel->deleteLater();
    bandTableModel->deleteLater();
    sotaCompleter->deleteLater();
    iotaCompleter->deleteLater();
    delete ui;
}
