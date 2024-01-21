#ifndef QLOG_UI_INPUTPASSWORDDIALOG_H
#define QLOG_UI_INPUTPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class InputPasswordDialog;
}

class InputPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputPasswordDialog(QString dialogName,
                                 QString comment,
                                 QWidget *parent = nullptr);
    ~InputPasswordDialog();

    QString getPassword() const;
    bool getRememberPassword() const;

private:
    Ui::InputPasswordDialog *ui;
};

#endif // QLOG_UI_INPUTPASSWORDDIALOG_H
