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
