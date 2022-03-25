#include <QStringListModel>

#include "RigWidget.h"
#include "ui_RigWidget.h"
#include "core/Rig.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.rigwidget");

RigWidget::RigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RigWidget)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    QStringListModel* rigModel = new QStringListModel(this);
    ui->rigProfilCombo->setModel(rigModel);
    refreshRigProfileCombo();

    Rig* rig = Rig::instance();
    connect(rig, &Rig::frequencyChanged, this, &RigWidget::updateFrequency);
    connect(rig, &Rig::modeChanged, this, &RigWidget::updateMode);
}

RigWidget::~RigWidget()
{
    FCT_IDENTIFICATION;

    delete ui;
}

void RigWidget::updateFrequency(double freq) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<freq;

    ui->freqLabel->setText(QString("%1 MHz").arg(QString::number(freq, 'f', 4)));
}

void RigWidget::updateMode(QString mode) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<mode;

    ui->modeLabel->setText(mode);
}

void RigWidget::rigProfileComboChanged(QString profileName)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << profileName;

    RigProfilesManager::instance()->setCurProfile1(profileName);
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

void RigWidget::reloadSettings()
{
    FCT_IDENTIFICATION;

    refreshRigProfileCombo();
}
