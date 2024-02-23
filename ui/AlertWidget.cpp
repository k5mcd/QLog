#include "AlertWidget.h"
#include "ui_AlertWidget.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.alertwidget");

//Maximal Aging interval is 20s
#define ALERT_AGING_CHECK_TIME 20000

AlertWidget::AlertWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlertWidget)
{
    FCT_IDENTIFICATION;
    ui->setupUi(this);

    alertTableModel = new AlertTableModel(this);

    ui->alertTableView->setModel(alertTableModel);
    ui->alertTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->alertTableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    QSettings settings;
    ui->clearAlertOlderSpinBox->setValue(settings.value("alert/alert_aging", 0).toInt());

    aging_timer = new QTimer;
    connect(aging_timer, &QTimer::timeout, this, &AlertWidget::alertAging);
    aging_timer->start(ALERT_AGING_CHECK_TIME);
}

AlertWidget::~AlertWidget()
{
    FCT_IDENTIFICATION;

    if ( aging_timer )
    {
        aging_timer->stop();
        aging_timer->deleteLater();
    }

    if ( alertTableModel )
    {
        alertTableModel->deleteLater();
    }

    delete ui;
}

void AlertWidget::addAlert(const SpotAlert &alert)
{
    FCT_IDENTIFICATION;

    alertTableModel->addAlert(alert);
    ui->alertTableView->repaint();
}

void AlertWidget::clearAllAlerts()
{
    FCT_IDENTIFICATION;

    alertTableModel->clear();
    emit alertsCleared();
}

void AlertWidget::entryDoubleClicked(QModelIndex index)
{
    FCT_IDENTIFICATION;
    emit tuneDx(alertTableModel->getCallsign(index),
                alertTableModel->getFrequency(index),
                alertTableModel->getBandPlanMode(index));
}

void AlertWidget::alertAgingChanged(int)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue("alert/alert_aging", ui->clearAlertOlderSpinBox->value());
}

void AlertWidget::alertAging()
{
    FCT_IDENTIFICATION;

    alertTableModel->aging(ui->clearAlertOlderSpinBox->value() * 60);
    ui->alertTableView->repaint();
    emit alertsCleared();
}

int AlertWidget::alertCount() const
{
    FCT_IDENTIFICATION;

    return alertTableModel->rowCount();
}
