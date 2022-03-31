#include <QStringListModel>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QLineEdit>

#include "RigWidget.h"
#include "ui_RigWidget.h"
#include "core/Rig.h"
#include "core/debug.h"
#include "models/SqlListModel.h"
#include "data/Data.h"

MODULE_IDENTIFICATION("qlog.ui.rigwidget");

RigWidget::RigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RigWidget)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    QStringListModel* rigModel = new QStringListModel(this);
    ui->rigProfilCombo->setModel(rigModel);
    ui->rigProfilCombo->setStyleSheet("QComboBox {color: red}");

    QSqlTableModel* bandComboModel = new QSqlTableModel();
    bandComboModel->setTable("bands");
    bandComboModel->setSort(bandComboModel->fieldIndex("start_freq"), Qt::AscendingOrder);
    ui->bandComboBox->setModel(bandComboModel);
    ui->bandComboBox->setModelColumn(bandComboModel->fieldIndex("name"));

    bandComboModel->select();

    QStringListModel* modesModel = new QStringListModel(this);
    ui->modeComboBox->setModel(modesModel);

    refreshRigProfileCombo();
    refreshBandCombo();
    refreshModeCombo();

    Rig* rig = Rig::instance();
    connect(rig, &Rig::frequencyChanged, this, &RigWidget::updateFrequency);
    connect(rig, &Rig::modeChanged, this, &RigWidget::updateMode);
    connect(rig, &Rig::vfoChanged, this, &RigWidget::updateVFO);
    connect(rig, &Rig::powerChanged, this, &RigWidget::updatePWR);
    connect(rig, &Rig::rigConnected, this, &RigWidget::rigConnected);
    connect(rig, &Rig::rigDisconnected, this, &RigWidget::rigDisconnected);
}

RigWidget::~RigWidget()
{
    FCT_IDENTIFICATION;

    delete ui;
}

void RigWidget::updateFrequency(double freq) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<freq;

    ui->freqLabel->setText(QString("%1 MHz").arg(QString::number(freq, 'f', 5)));
    if ( Data::band(freq).name != ui->bandComboBox->currentText() )
    {
        ui->bandComboBox->blockSignals(true);
        ui->bandComboBox->setCurrentText(Data::band(freq).name);
        ui->bandComboBox->blockSignals(false);
    }
}

void RigWidget::updateMode(QString rawMode, QString mode, QString submode)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<mode;

    Q_UNUSED(mode);
    Q_UNUSED(submode);

    ui->modeLabel->setText(rawMode);

    if ( mode != ui->modeComboBox->currentText() )
    {
        ui->modeComboBox->blockSignals(true);
        ui->modeComboBox->setCurrentText(rawMode);
        ui->modeComboBox->blockSignals(false);
    }
}

void RigWidget::updatePWR(double pwr)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<pwr;

    ui->pwrLabel->setText(QString(tr("PWR: %1W")).arg(pwr));
}

void RigWidget::updateVFO(unsigned int vfo)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<vfo;

    QString vfoString = " ";

    if ( vfo == RIG_VFO_A)
    {
        vfoString = tr("VFO-A");
    }
    else if ( vfo == RIG_VFO_B)
    {
        vfoString = tr("VFO-B");
    }
    else if ( vfo == RIG_VFO_C)
    {
        vfoString = tr("VFO-C");
    }
    else if ( vfo == RIG_VFO_SUB_A)
    {
        vfoString = tr("VFO-SubA");
    }
    else if ( vfo == RIG_VFO_SUB_B)
    {
        vfoString = tr("VFO-SubB");
    }
    else if ( vfo == RIG_VFO_MAIN_A)
    {
        vfoString = tr("VFO-MainA");
    }
    else if ( vfo == RIG_VFO_MAIN_B)
    {
        vfoString = tr("VFO-MainB");
    }
    else if ( vfo == RIG_VFO_SUB)
    {
        vfoString = tr("VFO-Sub");
    }
    else if ( vfo == RIG_VFO_MAIN)
    {
        vfoString = tr("VFO-Main");
    }
    else if ( vfo == RIG_VFO_VFO)
    {
        vfoString = tr("VFO");
    }
    else if ( vfo == RIG_VFO_MEM)
    {
        vfoString = tr("VFO-Mem");
    }
    else if ( vfo == RIG_VFO_CURR)
    {
        vfoString = tr("VFO");
    }

    ui->vfoLabel->setText(vfoString);
}

void RigWidget::bandComboChanged(QString newBand)
{
    FCT_IDENTIFICATION;

    qCDebug(runtime) << "new Band:" << newBand;

    QSqlTableModel* bandComboModel = dynamic_cast<QSqlTableModel*>(ui->bandComboBox->model());
    QSqlRecord record = bandComboModel->record(ui->bandComboBox->currentIndex());

    double newFreq = record.value("start_freq").toDouble();

    qCDebug(runtime) << "Tunning freq: " << newFreq;

    Rig::instance()->setFrequency(newFreq);
}

void RigWidget::modeComboChanged(QString newMode)
{
    FCT_IDENTIFICATION;

    Rig::instance()->setMode(newMode);
}

void RigWidget::rigProfileComboChanged(QString profileName)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << profileName;

    RigProfilesManager::instance()->setCurProfile1(profileName);
    refreshBandCombo();
    refreshModeCombo();
    resetRigInfo();
    emit rigProfileChanged();
}

void RigWidget::refreshRigProfileCombo()
{
    ui->rigProfilCombo->blockSignals(true);

    QStringList currProfiles = RigProfilesManager::instance()->profileNameList();
    QStringListModel* model = dynamic_cast<QStringListModel*>(ui->rigProfilCombo->model());

    model->setStringList(currProfiles);

    if ( RigProfilesManager::instance()->getCurProfile1().profileName.isEmpty()
         && currProfiles.count() > 0 )
    {
        /* changing profile from empty to something */
        ui->rigProfilCombo->setCurrentText(currProfiles.first());
        rigProfileComboChanged(currProfiles.first());
    }
    else
    {
        /* no profile change, just refresh the combo and preserve current profile */
        ui->rigProfilCombo->setCurrentText(RigProfilesManager::instance()->getCurProfile1().profileName);
    }

    ui->rigProfilCombo->blockSignals(false);
}

void RigWidget::refreshBandCombo()
{
    FCT_IDENTIFICATION;

    QString currSelection = ui->bandComboBox->currentText();
    RigProfile profile = RigProfilesManager::instance()->getCurProfile1();

    ui->bandComboBox->blockSignals(true);
    ui->bandComboBox->clear();

    QSqlTableModel *bandComboModel = dynamic_cast<QSqlTableModel*>(ui->bandComboBox->model());
    bandComboModel->setFilter(QString("enabled = 1 AND start_freq >= %1 AND end_freq <= %2").arg(profile.txFreqStart).arg(profile.txFreqEnd));
    dynamic_cast<QSqlTableModel*>(ui->bandComboBox->model())->select();

    ui->bandComboBox->setCurrentText(currSelection);
    ui->bandComboBox->blockSignals(false);
}

void RigWidget::refreshModeCombo()
{
    FCT_IDENTIFICATION;

    QString currSelection = ui->modeComboBox->currentText();

    ui->modeComboBox->blockSignals(true);
    ui->modeComboBox->clear();

    QStringListModel* model = dynamic_cast<QStringListModel*>(ui->modeComboBox->model());
    model->setStringList(Rig::instance()->getAvailableModes());

    ui->modeComboBox->setCurrentText(currSelection);
    ui->modeComboBox->blockSignals(false);
}

void RigWidget::reloadSettings()
{
    FCT_IDENTIFICATION;

    refreshRigProfileCombo();
    refreshBandCombo();
    refreshModeCombo();
}

void RigWidget::rigConnected()
{
    FCT_IDENTIFICATION;

    ui->rigProfilCombo->setStyleSheet("QComboBox {color: green}");
}

void RigWidget::rigDisconnected()
{
    FCT_IDENTIFICATION;

    ui->rigProfilCombo->setStyleSheet("QComboBox {color: red}");
    resetRigInfo();
}

void RigWidget::resetRigInfo()
{
    QString empty;

    updateMode(empty, empty, empty);
    ui->pwrLabel->setText(QString(""));
    updateVFO(-1);
    updateFrequency(0);
}
