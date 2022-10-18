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
class ColumnSettingSimpleDialog;
}

class ColumnSettingGenericDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColumnSettingGenericDialog(QTableView *table, QWidget *parent = nullptr);
    ~ColumnSettingGenericDialog(){};

protected:
    void addSortedCheckboxes(QGridLayout *grid, QList<QCheckBox*> &checkboxlist, int elementsPerRow);
    void addSelectUnselect(QGridLayout *, int);
    QTableView table;
};

class ColumnSettingSimpleDialog : public ColumnSettingGenericDialog
{
    Q_OBJECT

public:
    explicit ColumnSettingSimpleDialog(QTableView *table, QWidget *parent = nullptr);
    ~ColumnSettingSimpleDialog();

private:
    Ui::ColumnSettingSimpleDialog *ui;
};

class ColumnSettingDialog : public ColumnSettingGenericDialog
{
    Q_OBJECT

public:
    explicit ColumnSettingDialog(QTableView *table, QWidget *parent = nullptr);
    ~ColumnSettingDialog();

private:

    Ui::ColumnSettingDialog *ui;
};

#endif // COLUMNSETTINGDIALOG_H
