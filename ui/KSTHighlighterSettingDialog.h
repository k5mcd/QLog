#ifndef QLOG_UI_KSTHIGHLIGHTERSETTINGDIALOG_H
#define QLOG_UI_KSTHIGHLIGHTERSETTINGDIALOG_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class KSTHighlighterSettingDialog;
}

class KSTHighlighterSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KSTHighlighterSettingDialog(QWidget *parent = nullptr);
    ~KSTHighlighterSettingDialog();

    QSqlTableModel* rulesModel;

public slots:
    void addRule();
    void removeRule();
    void editRule(QModelIndex);
    void editRuleButton();

private:
    Ui::KSTHighlighterSettingDialog *ui;
};

#endif // QLOG_UI_KSTHIGHLIGHTERSETTINGDIALOG_H
