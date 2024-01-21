#ifndef QLOG_UI_EDITLAYOUTDIALOG_H
#define QLOG_UI_EDITLAYOUTDIALOG_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class EditLayoutDialog;
}

class EditLayoutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditLayoutDialog(QWidget *parent = nullptr);
    ~EditLayoutDialog();

private:
    Ui::EditLayoutDialog *ui;

    void loadProfiles();

public slots:
    void addButton();
    void removeButton();
    void editEvent(QModelIndex);
    void editButton();
};

#endif // QLOG_UI_EDITLAYOUTDIALOG_H
