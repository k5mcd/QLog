#include "AlertSettingDialog.h"
#include "ui_AlertSettingDialog.h"
#include "core/debug.h"
#include "ui/AlertRuleDetail.h"

MODULE_IDENTIFICATION("qlog.ui.alertsettingdialog");

AlertSettingDialog::AlertSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlertSettingDialog)
{
    ui->setupUi(this);

    rulesModel = new QSqlTableModel();
    rulesModel->setTable("alert_rules");
    ui->rulesListView->setModel(rulesModel);
    ui->rulesListView->setModelColumn(rulesModel->fieldIndex("rule_name"));
    ui->rulesListView->setSelectionMode(QAbstractItemView::SingleSelection);
    rulesModel->select();
}

AlertSettingDialog::~AlertSettingDialog()
{
    delete ui;
}

void AlertSettingDialog::addRule()
{
    FCT_IDENTIFICATION;
    AlertRuleDetail dialog(QString(), this);
    dialog.exec();
    rulesModel->select();
}

void AlertSettingDialog::removeRule()
{
    FCT_IDENTIFICATION;

    rulesModel->removeRow(ui->rulesListView->currentIndex().row());
    ui->rulesListView->clearSelection();
    rulesModel->select();
}

void AlertSettingDialog::editRule(QModelIndex idx)
{
    FCT_IDENTIFICATION;

    QString filterName = ui->rulesListView->model()->data(idx).toString();

    AlertRuleDetail dialog(filterName, this);
    dialog.exec();

}

void AlertSettingDialog::editRuleButton()
{
    FCT_IDENTIFICATION;

    foreach (QModelIndex index, ui->rulesListView->selectionModel()->selectedIndexes())
    {
       editRule(index);
    }
}
