#include <QDebug>
#include <QSettings>

#include "DxFilterDialog.h"
#include "ui_DxFilterDialog.h"

DxFilterDialog::DxFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DxFilterDialog)
{
    ui->setupUi(this);

    QSettings settings;

    ui->cwcheckbox->setChecked(settings.value("dxc/filter_mode_cw",true).toBool());
    ui->phonecheckbox->setChecked(settings.value("dxc/filter_mode_phone",true).toBool());
    ui->digitalcheckbox->setChecked(settings.value("dxc/filter_mode_digital",true).toBool());
    ui->ft8checkbox->setChecked(settings.value("dxc/filter_mode_ft8",true).toBool());

    QString contregexp = settings.value("dxc/filter_cont_regexp", "NOTHING|AF|AN|AS|EU|NA|OC|SA").toString();

    ui->afcheckbox->setChecked(contregexp.contains("|AF"));
    ui->ancheckbox->setChecked(contregexp.contains("|AN"));
    ui->ascheckbox->setChecked(contregexp.contains("|AS"));
    ui->eucheckbox->setChecked(contregexp.contains("|EU"));
    ui->nacheckbox->setChecked(contregexp.contains("|NA"));
    ui->occheckbox->setChecked(contregexp.contains("|OC"));
    ui->sacheckbox->setChecked(contregexp.contains("|SA"));

    QString contregexp_spotter = settings.value("dxc/filter_spotter_cont_regexp", "NOTHING|AF|AN|AS|EU|NA|OC|SA").toString();

    ui->afcheckbox_spotter->setChecked(contregexp_spotter.contains("|AF"));
    ui->ancheckbox_spotter->setChecked(contregexp_spotter.contains("|AN"));
    ui->ascheckbox_spotter->setChecked(contregexp_spotter.contains("|AS"));
    ui->eucheckbox_spotter->setChecked(contregexp_spotter.contains("|EU"));
    ui->nacheckbox_spotter->setChecked(contregexp_spotter.contains("|NA"));
    ui->occheckbox_spotter->setChecked(contregexp_spotter.contains("|OC"));
    ui->sacheckbox_spotter->setChecked(contregexp_spotter.contains("|SA"));
}

void DxFilterDialog::accept()
{
    QSettings settings;

    settings.setValue("dxc/filter_mode_cw", ui->cwcheckbox->isChecked());
    settings.setValue("dxc/filter_mode_phone", ui->phonecheckbox->isChecked());
    settings.setValue("dxc/filter_mode_digital", ui->digitalcheckbox->isChecked());
    settings.setValue("dxc/filter_mode_ft8", ui->ft8checkbox->isChecked());

    QString contregexp = "NOTHING";
    if ( ui->afcheckbox->isChecked() ) contregexp.append("|AF");
    if ( ui->ancheckbox->isChecked() ) contregexp.append("|AN");
    if ( ui->ascheckbox->isChecked() ) contregexp.append("|AS");
    if ( ui->eucheckbox->isChecked() ) contregexp.append("|EU");
    if ( ui->nacheckbox->isChecked() ) contregexp.append("|NA");
    if ( ui->occheckbox->isChecked() ) contregexp.append("|OC");
    if ( ui->sacheckbox->isChecked() ) contregexp.append("|SA");
    settings.setValue("dxc/filter_cont_regexp", contregexp);

    QString contregexp_spotter = "NOTHING";
    if ( ui->afcheckbox_spotter->isChecked() ) contregexp_spotter.append("|AF");
    if ( ui->ancheckbox_spotter->isChecked() ) contregexp_spotter.append("|AN");
    if ( ui->ascheckbox_spotter->isChecked() ) contregexp_spotter.append("|AS");
    if ( ui->eucheckbox_spotter->isChecked() ) contregexp_spotter.append("|EU");
    if ( ui->nacheckbox_spotter->isChecked() ) contregexp_spotter.append("|NA");
    if ( ui->occheckbox_spotter->isChecked() ) contregexp_spotter.append("|OC");
    if ( ui->sacheckbox_spotter->isChecked() ) contregexp_spotter.append("|SA");
    settings.setValue("dxc/filter_spotter_cont_regexp", contregexp_spotter);

    done(QDialog::Accepted);
}

DxFilterDialog::~DxFilterDialog()
{
    delete ui;
}
