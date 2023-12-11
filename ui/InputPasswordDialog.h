#ifndef INPUTPASSWORDDIALOG_H
#define INPUTPASSWORDDIALOG_H

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

#endif // INPUTPASSWORDDIALOG_H
