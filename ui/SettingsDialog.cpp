#include <QSettings>
#include <QStringListModel>
#include <QSqlTableModel>
#include <hamlib/rig.h>
#include <hamlib/rotator.h>

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "models/RigTypeModel.h"
#include "models/RotTypeModel.h"
#include "../core/utils.h"
#include "../core/HamQTH.h"
#include "../core/Lotw.h"
#include "../core/ClubLog.h"
#include "../core/StyleItemDelegate.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.settingdialog");

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
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
    FCT_IDENTIFICATION;

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

void SettingsDialog::readSettings() {
    FCT_IDENTIFICATION;

    QSettings settings;
    QString username;

    ui->callsignEdit->setText(settings.value("station/callsign").toString());
    ui->locatorEdit->setText(settings.value("station/grid").toString());
    ui->operatorEdit->setText(settings.value("station/operator").toString());
    ui->qthEdit->setText(settings.value("station/qth").toString());
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
    ui->rotNetPortSpin->setDisabled(true);
}

void SettingsDialog::writeSettings() {
    FCT_IDENTIFICATION;

    QSettings settings;
    QString old_username;

    settings.setValue("station/callsign", ui->callsignEdit->text());
    settings.setValue("station/grid", ui->locatorEdit->text());
    settings.setValue("station/operator", ui->operatorEdit->text());
    settings.setValue("station/qth", ui->qthEdit->text());

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
    FCT_IDENTIFICATION;

    delete modeTableModel;
    delete bandTableModel;
    delete ui;
}
