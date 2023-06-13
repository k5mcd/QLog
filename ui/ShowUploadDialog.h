#ifndef SHOWUPLOADDIALOG_H
#define SHOWUPLOADDIALOG_H

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

#endif // SHOWUPLOADDIALOG_H
