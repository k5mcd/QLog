#ifndef QRZDIALOG_H
#define QRZDIALOG_H

#include <QDialog>
#include "core/LogLocale.h"

namespace Ui {
class QRZDialog;
}

class QRZDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QRZDialog(QWidget *parent = nullptr);
    ~QRZDialog();

public slots:
    void upload();
    void uploadCallsignChanged(const QString&);

private:
    Ui::QRZDialog *ui;

    void saveDialogState();
    void loadDialogState();

    LogLocale locale;
};

#endif // QRZDIALOG_H
