#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QSqlRecord>
#include <QList>

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = nullptr);
    explicit ExportDialog(const QList<QSqlRecord>&, QWidget *parent = nullptr);
    ~ExportDialog();

public slots:
    void browse();
    void toggleAll();
    void runExport();

private:
    Ui::ExportDialog *ui;

    const QList<QSqlRecord> qsos4export;
    void setProgress(float);
};

#endif // EXPORTDIALOG_H
