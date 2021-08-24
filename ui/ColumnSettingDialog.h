#ifndef COLUMNSETTINGDIALOG_H
#define COLUMNSETTINGDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QGridLayout>
#include <QCheckBox>
#include <QPair>
#include <QList>
#include <QString>

namespace Ui {
class ColumnSettingDialog;
}

class ColumnSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColumnSettingDialog(QTableView *table, QWidget *parent = nullptr);
    ~ColumnSettingDialog();


private:
    void addSortedCheckboxes(QGridLayout *grid, QList<QCheckBox*> &checkboxlist, int elementsPerRow);
    void addSelectUnselect(QGridLayout *, int);

    Ui::ColumnSettingDialog *ui;
    QTableView table;
};

#endif // COLUMNSETTINGDIALOG_H
