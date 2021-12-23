#ifndef LOTWDIALOG_H
#define LOTWDIALOG_H

#include <QDialog>

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
};

#endif // LOTWDIALOG_H
