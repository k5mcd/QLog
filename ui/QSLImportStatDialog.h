#ifndef QLOG_UI_QSLIMPORTSTATDIALOG_H
#define QLOG_UI_QSLIMPORTSTATDIALOG_H

#include <QDialog>
#include <logformat/LogFormat.h>

namespace Ui {
class QSLImportStatDialog;
}

class QSLImportStatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QSLImportStatDialog(QSLMergeStat stats, QWidget *parent = nullptr);
    ~QSLImportStatDialog();

private:
    Ui::QSLImportStatDialog *ui;
};

#endif // QLOG_UI_QSLIMPORTSTATDIALOG_H
