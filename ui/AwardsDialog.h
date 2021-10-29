#ifndef AWARDSDIALOG_H
#define AWARDSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include "models/AwardsTableModel.h"

namespace Ui {
class AwardsDialog;
}

class AwardsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AwardsDialog(QWidget *parent = nullptr);
    ~AwardsDialog();
public slots:
    void refreshTable(int);

private:
    Ui::AwardsDialog *ui;
    AwardsTableModel *detailedViewModel;

};

#endif // AWARDSDIALOG_H
