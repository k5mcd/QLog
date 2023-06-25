#include "ShowUploadDialog.h"
#include "ui_ShowUploadDialog.h"
#include <QPushButton>

ShowUploadDialog::ShowUploadDialog(const QString &qsoList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowUploadDialog)
{
    ui->setupUi(this);
    ui->qsoText->insertPlainText(qsoList);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Upload"));
}

ShowUploadDialog::~ShowUploadDialog()
{
    delete ui;
}
