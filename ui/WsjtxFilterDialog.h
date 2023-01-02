#ifndef WSJTXFILTERDIALOG_H
#define WSJTXFILTERDIALOG_H

#include <QDialog>

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
};

#endif // WSJTXFILTERDIALOG_H
