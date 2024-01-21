#ifndef QLOG_UI_LOTWDIALOG_H
#define QLOG_UI_LOTWDIALOG_H

#include <QDialog>
#include "core/LogLocale.h"

namespace Ui {
class LotwDialog;
}

class LotwDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LotwDialog(QWidget *parent = nullptr);
    ~LotwDialog();

public slots:
    void download();
    void upload();
    void uploadCallsignChanged(const QString& );

private:
    Ui::LotwDialog *ui;

    void saveDialogState();
    void loadDialogState();
    LogLocale locale;
};

#endif // QLOG_UI_LOTWDIALOG_H
