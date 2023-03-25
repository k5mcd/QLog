#ifndef DXFILTER_H
#define DXFILTER_H

#include <QDialog>
#include <QCheckBox>

namespace Ui {
class DxFilterDialog;
}

class DxFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DxFilterDialog(QWidget *parent = nullptr);
    ~DxFilterDialog();
    void accept() override;

private:
    Ui::DxFilterDialog *ui;
    QList<QCheckBox*> memberListCheckBoxes;

    void generateMembershipCheckboxes();

};

#endif // DXFILTER_H
