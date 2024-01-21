#ifndef QLOG_UI_HRDLOGDIALOG_H
#define QLOG_UI_HRDLOGDIALOG_H

#include <QDialog>
#include "core/LogLocale.h"

namespace Ui {
class HRDLogDialog;
}

class HRDLogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HRDLogDialog(QWidget *parent = nullptr);
    ~HRDLogDialog();

public slots:
    void upload();
    void uploadCallsignChanged(const QString&);

private:
    Ui::HRDLogDialog *ui;

    void saveDialogState();
    void loadDialogState();
    LogLocale locale;
};

#endif // QLOG_UI_HRDLOGDIALOG_H
