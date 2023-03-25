#ifndef WSJTXFILTERDIALOG_H
#define WSJTXFILTERDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QSet>

namespace Ui {
class WsjtxFilterDialog;
}

class WsjtxFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WsjtxFilterDialog(QWidget *parent = nullptr);
    ~WsjtxFilterDialog();
    void accept() override;

private:
    Ui::WsjtxFilterDialog *ui;
    QList<QCheckBox*> memberListCheckBoxes;
    QSet<QString> dxMemberFilter;

    void generateMembershipCheckboxes();

};

#endif // WSJTXFILTERDIALOG_H
