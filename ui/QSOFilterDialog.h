#ifndef QSOFILTERDIALOG_H
#define QSOFILTERDIALOG_H

#include <QSqlTableModel>
#include <QDialog>


namespace Ui {
class QSOFilterDialog;
}

class QSOFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSOFilterDialog(QWidget *parent = nullptr);
    ~QSOFilterDialog();

private:
    Ui::QSOFilterDialog *ui;
    QSqlTableModel* filterModel;

public slots:
    void addFilter();
    void removeFilter();
    void editFilter(QModelIndex);
    void editFilterButton();
};

#endif // QSOFILTERDIALOG_H
