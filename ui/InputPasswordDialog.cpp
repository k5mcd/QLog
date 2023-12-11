#include "InputPasswordDialog.h"
#include "ui_InputPasswordDialog.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.dxwidget");

InputPasswordDialog::InputPasswordDialog(QString dialogName,
                                         QString comment,
                                         QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputPasswordDialog)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    setWindowTitle(dialogName);
    ui->comment->setText(comment);
}

InputPasswordDialog::~InputPasswordDialog()
{
    FCT_IDENTIFICATION;

    delete ui;
}

QString InputPasswordDialog::getPassword() const
{
    FCT_IDENTIFICATION;
    return ui->passwordLineEdit->text();
}

bool InputPasswordDialog::getRememberPassword() const
{
    FCT_IDENTIFICATION;
    return ui->rememberCheckBox->isChecked();
}
