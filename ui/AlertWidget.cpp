#include "AlertWidget.h"
#include "ui_AlertWidget.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.alertwidget");

AlertWidget::AlertWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlertWidget)
{
    FCT_IDENTIFICATION;
    ui->setupUi(this);

    alertTableModel = new AlertTableModel(this);

    ui->alertTableView->setModel(alertTableModel);
    ui->alertTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

AlertWidget::~AlertWidget()
{
    FCT_IDENTIFICATION;
    delete ui;
}

void AlertWidget::addAlert(const UserAlert &alert)
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


    QString callsign = alertTableModel->getCallsign(index);
    double frequency = alertTableModel->getFrequency(index);
    emit tuneDx(callsign, frequency);
}

int AlertWidget::alertCount() const
{
    FCT_IDENTIFICATION;

    return alertTableModel->rowCount();
}
