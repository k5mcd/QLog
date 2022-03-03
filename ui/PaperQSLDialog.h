#ifndef PAPERQSLDIALOG_H
#define PAPERQSLDIALOG_H

#include <QDialog>
#include <QSqlRecord>

#include "core/PaperQSL.h"

namespace Ui {
class PaperQSLDialog;
}

class PaperQSLDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PaperQSLDialog(const QSqlRecord &qso, QWidget *parent = nullptr);
    ~PaperQSLDialog();

public slots:
    void addFileClick();

private:

    void showAvailableFiles();
    void addFileToDialog(const QFileInfo &);
    void addNewFile(const QString &);

    Ui::PaperQSLDialog *ui;

    PaperQSL *qsl;
    QSqlRecord dialogQSORecord;
    unsigned int fileCount;
};

#endif // PAPERQSLDIALOG_H
