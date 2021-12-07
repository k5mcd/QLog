#ifndef CLUBLOGDIALOG_H
#define CLUBLOGDIALOG_H

#include <QDialog>

namespace Ui {
class ClublogDialog;
}

class ClublogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClublogDialog(QWidget *parent = nullptr);
    ~ClublogDialog();

public slots:
    void upload();
    void uploadCallsignChanged(QString );


private:
    Ui::ClublogDialog *ui;
};

#endif // CLUBLOGDIALOG_H
