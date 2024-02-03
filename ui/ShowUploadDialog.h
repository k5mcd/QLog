#ifndef QLOG_UI_SHOWUPLOADDIALOG_H
#define QLOG_UI_SHOWUPLOADDIALOG_H

#include <QDialog>

namespace Ui {
class ShowUploadDialog;
}

class ShowUploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShowUploadDialog(const QString &qsoList, QWidget *parent = nullptr);
    ~ShowUploadDialog();

private:
    Ui::ShowUploadDialog *ui;
};

#endif // QLOG_UI_SHOWUPLOADDIALOG_H
