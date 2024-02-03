#ifndef QLOG_UI_CLUBLOGDIALOG_H
#define QLOG_UI_CLUBLOGDIALOG_H

#include <QDialog>
#include "core/LogLocale.h"

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
    void uploadCallsignChanged(const QString&);


private:
    Ui::ClublogDialog *ui;

    void saveDialogState();
    void loadDialogState();
    LogLocale locale;
};

#endif // QLOG_UI_CLUBLOGDIALOG_H
