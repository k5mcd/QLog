#include "LotwShowUploadDialog.h"
#include "ui_LotwShowUploadDialog.h"
#include <QPushButton>

LotwShowUploadDialog::LotwShowUploadDialog(const QString &qsoList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LotwShowUploadDialog)
{
    ui->setupUi(this);
    ui->qsoText->insertPlainText(qsoList);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Upload"));
}

LotwShowUploadDialog::~LotwShowUploadDialog()
{
    delete ui;
}
