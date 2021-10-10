#ifndef EQSLDIALOG_H
#define EQSLDIALOG_H

#include <QDialog>

namespace Ui {
class EqslDialog;
}

class EqslDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EqslDialog(QWidget *parent = nullptr);
    ~EqslDialog();

public slots:
    void download();
    void upload();
    void uploadCallsignChanged(QString );

private:
    Ui::EqslDialog *ui;
};

#endif // EQSLDIALOG_H
