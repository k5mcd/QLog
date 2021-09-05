#include "QSLImportStatDialog.h"
#include "ui_QSLImportStatDialog.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.QSLImportStatDialog");

QSLImportStatDialog::QSLImportStatDialog(QSLMergeStat stats, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSLImportStatDialog)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    ui->updatedNumer->setText(QString::number(stats.qsos_updated));
    ui->downloadedNumber->setText(QString::number(stats.qsos_checked));
    ui->unmatchedNumber->setText(QString::number(stats.qsos_unmatched));
    ui->errorsNumber->setText(QString::number(stats.qsos_errors));

    ui->detailsText->moveCursor(QTextCursor::End);
    ui->detailsText->insertPlainText (tr("New QSLs: \n)"));
    ui->detailsText->moveCursor(QTextCursor::End);
    ui->detailsText->insertPlainText (stats.newQSLs.join(", "));
    ui->detailsText->moveCursor (QTextCursor::End);

    ui->detailsText->insertPlainText(tr("\nUnmatched QSLs: \n)"));
    ui->detailsText->moveCursor(QTextCursor::End);
    ui->detailsText->insertPlainText (stats.unmatchedQSLs.join(", "));
    ui->detailsText->moveCursor (QTextCursor::End);
}

QSLImportStatDialog::~QSLImportStatDialog()
{
    FCT_IDENTIFICATION;

    delete ui;
}
