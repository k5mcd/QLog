#ifndef ALERTSETTINGDIALOG_H
#define ALERTSETTINGDIALOG_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class AlertSettingDialog;
}

class AlertSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlertSettingDialog(QWidget *parent = nullptr);
    ~AlertSettingDialog();

private:
    Ui::AlertSettingDialog *ui;
    QSqlTableModel* rulesModel;

public slots:
    void addRule();
    void removeRule();
    void editRule(QModelIndex);
    void editRuleButton();

};

#endif // ALERTSETTINGDIALOG_H
