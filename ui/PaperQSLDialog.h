#ifndef PAPERQSLDIALOG_H
#define PAPERQSLDIALOG_H

#include <QDialog>
#include <QSqlRecord>

#include "core/QSLStorage.h"

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
    void addFileToDialog(const QString &);
    void addNewFile(const QString &);

    Ui::PaperQSLDialog *ui;

    QSLStorage *qsl;
    QSqlRecord dialogQSORecord;
    unsigned int fileCount;
    QStringList filenameList;
};

#endif // PAPERQSLDIALOG_H
