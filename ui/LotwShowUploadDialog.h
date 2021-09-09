#ifndef LOTWSHOWUPLOADDIALOG_H
#define LOTWSHOWUPLOADDIALOG_H

#include <QDialog>

namespace Ui {
class LotwShowUploadDialog;
}

class LotwShowUploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LotwShowUploadDialog(QString qsoList, QWidget *parent = nullptr);
    ~LotwShowUploadDialog();

private:
    Ui::LotwShowUploadDialog *ui;
};

#endif // LOTWSHOWUPLOADDIALOG_H
