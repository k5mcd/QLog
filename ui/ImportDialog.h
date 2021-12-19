#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include <QSqlRecord>
#include <logformat/LogFormat.h>

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

public slots:
    void browse();
    void toggleAll();
    void toggleMyGrid();
    void toggleMyRig();
    void toggleComment();
    void adjustLocatorTextColor();
    void runImport();
    void progress(qint64 value);

private:
    Ui::ImportDialog *ui;
    qint64 size;

    static LogFormat::duplicateQSOBehaviour showDuplicateDialog(QSqlRecord *, QSqlRecord *);
};

#endif // IMPORTDIALOG_H
