#include <QSettings>
#include <QStringListModel>
#include <QSqlTableModel>
#include <hamlib/rig.h>

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "models/RigTypeModel.h"
#include "models/RotTypeModel.h"
#include "../core/utils.h"
#include "../core/HamQTH.h"
#include "../core/Lotw.h"
#include "../core/ClubLog.h"
#include "../core/StyleItemDelegate.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    RigTypeModel* rigTypeModel = new RigTypeModel(this);
    ui->rigModelSelect->setModel(rigTypeModel);

    RotTypeModel* rotTypeModel = new RotTypeModel(this);
    ui->rotModelSelect->setModel(rotTypeModel);

    QStringListModel* rigModel = new QStringListModel();
    ui->rigListView->setModel(rigModel);

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

    bandTableModel->select();

    readSettings();
}

void SettingsDialog::save() {
    writeSettings();
    accept();
}

void SettingsDialog::addRig() {
    if (ui->rigNameEdit->text().isEmpty()) return;

    QStringListModel* model = (QStringListModel*)ui->rigListView->model();
    QStringList rigs = model->stringList();
    rigs << ui->rigNameEdit->text();
    model->setStringList(rigs);
    ui->rigNameEdit->clear();
}

void SettingsDialog::deleteRig() {
    foreach (QModelIndex index, ui->rigListView->selectionModel()->selectedRows()) {
        ui->rigListView->model()->removeRow(index.row());
    }
    ui->rigListView->clearSelection();
}

void SettingsDialog::rigChanged(int index)
{
    const struct rig_caps *caps;

    QModelIndex rig_index = ui->rigModelSelect->model()->index(index, 0);
    caps = rig_get_caps(rig_index.internalId());

    if ( caps )
    {
        if ( caps->port_type == RIG_PORT_NETWORK
             || caps->port_type == RIG_PORT_UDP_NETWORK)
        {
            ui->stackedWidget->setCurrentIndex(1);
        }
        else
        {
            ui->stackedWidget->setCurrentIndex(0);
        }
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void SettingsDialog::readSettings() {
    QSettings settings;
    QString username;

    ui->callsignEdit->setText(settings.value("station/callsign").toString());
    ui->locatorEdit->setText(settings.value("station/grid").toString());
    ui->operatorEdit->setText(settings.value("station/operator").toString());
    QStringList rigs = settings.value("station/rigs").toStringList();
    ((QStringListModel*)ui->rigListView->model())->setStringList(rigs);

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

    username = settings.value(HamQTH::CONFIG_USERNAME_KEY).toString();
    ui->hamQthUsernameEdit->setText(username);
    ui->hamQthPasswordEdit->setText(getPassword(HamQTH::SECURE_STORAGE_KEY, username));

    username = settings.value(Lotw::CONFIG_USERNAME_KEY).toString();
    ui->lotwUsernameEdit->setText(username);
    ui->lotwPasswordEdit->setText(getPassword(Lotw::SECURE_STORAGE_KEY, username));

    username = settings.value(ClubLog::CONFIG_EMAIL_KEY).toString();
    ui->clublogEmailEdit->setText(username);
    ui->clublogCallsignEdit->setText(settings.value(ClubLog::CONFIG_CALLSIGN_KEY).toString());
    ui->clublogPasswordEdit->setText(getPassword(ClubLog::SECURE_STORAGE_KEY, username));

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
}

void SettingsDialog::writeSettings() {
    QSettings settings;
    QString old_username;

    settings.setValue("station/callsign", ui->callsignEdit->text());
    settings.setValue("station/grid", ui->locatorEdit->text());
    settings.setValue("station/operator", ui->operatorEdit->text());
    QStringList rigs = ((QStringListModel*)ui->rigListView->model())->stringList();
    settings.setValue("station/rigs", rigs);

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

    /**********/
    /* HamQTH */
    /**********/
    old_username = settings.value(HamQTH::CONFIG_USERNAME_KEY).toString();
    if ( old_username != ui->hamQthUsernameEdit->text() )
    {
        deletePassword(HamQTH::SECURE_STORAGE_KEY,old_username);
    }

    settings.setValue(HamQTH::CONFIG_USERNAME_KEY, ui->hamQthUsernameEdit->text());
    savePassword(HamQTH::SECURE_STORAGE_KEY, ui->hamQthUsernameEdit->text(), ui->hamQthPasswordEdit->text());

    /********/
    /* LoTW */
    /********/
    old_username = settings.value(Lotw::CONFIG_USERNAME_KEY).toString();
    if ( old_username != ui->lotwUsernameEdit->text() )
    {
        deletePassword(Lotw::SECURE_STORAGE_KEY,old_username);
    }
    settings.setValue(Lotw::CONFIG_USERNAME_KEY, ui->lotwUsernameEdit->text());
    savePassword(Lotw::SECURE_STORAGE_KEY, ui->lotwUsernameEdit->text(), ui->lotwPasswordEdit->text());

    /***********/
    /* ClubLog */
    /***********/
    old_username = settings.value(ClubLog::CONFIG_EMAIL_KEY).toString();
    if ( old_username != ui->clublogEmailEdit->text() )
    {
        deletePassword(ClubLog::SECURE_STORAGE_KEY,old_username);
    }
    settings.setValue(ClubLog::CONFIG_EMAIL_KEY, ui->clublogEmailEdit->text());
    settings.setValue(ClubLog::CONFIG_CALLSIGN_KEY, ui->clublogCallsignEdit->text());
    savePassword(ClubLog::SECURE_STORAGE_KEY, ui->clublogEmailEdit->text(),ui->clublogPasswordEdit->text());

    if (ui->dxccStartDateCheckBox->isChecked()) {
        settings.setValue("dxcc/start", ui->dxccStartDate->date());
    }
    else {
        settings.setValue("dxcc/start", QVariant());
    }
}

SettingsDialog::~SettingsDialog() {
    delete modeTableModel;
    delete bandTableModel;
    delete ui;
}
