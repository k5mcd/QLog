#include <QtSql/QtSql>
#include <QShortcut>
#include <QDesktopServices>
#include <QDebug>
#include <QCompleter>
#include <QMessageBox>
#include "core/Rig.h"
#include "NewContactWidget.h"
#include "ui_NewContactWidget.h"
#include "core/debug.h"
#include "core/Gridsquare.h"
#include "data/StationProfile.h"
#include "core/HamQTH.h"
#include "core/QRZ.h"

MODULE_IDENTIFICATION("qlog.ui.newcontactwidget");

NewContactWidget::NewContactWidget(QWidget *parent) :
    QWidget(parent),
    primaryCallbook(nullptr),
    secondaryCallbook(nullptr),
    ui(new Ui::NewContactWidget),
    prop_cond(nullptr)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    ui->qslSentBox->addItem(tr("No"), QVariant("N"));
    ui->qslSentBox->addItem(tr("Yes"), QVariant("Y"));
    ui->qslSentBox->addItem(tr("Requested"), QVariant("R"));
    ui->qslSentBox->addItem(tr("Queued"), QVariant("Q"));
    ui->qslSentBox->addItem(tr("Ignored"), QVariant("I"));

    ui->qslSentViaBox->addItem("", QVariant(""));
    ui->qslSentViaBox->addItem(tr("Bureau"), QVariant("B"));
    ui->qslSentViaBox->addItem(tr("Direct"), QVariant("D"));
    ui->qslSentViaBox->addItem(tr("Electronic"), QVariant("E"));

    rig = Rig::instance();

    QStringListModel* rigModel = new QStringListModel(this);
    ui->rigEdit->setModel(rigModel);

    QStringListModel* antModel = new QStringListModel(this);
    ui->antennaEdit->setModel(antModel);

    QStringListModel* submodeModel = new QStringListModel(this);
    ui->submodeEdit->setModel(submodeModel);

    QStringListModel* stationProfilesModel = new QStringListModel(this);
    ui->stationProfileCombo->setModel(stationProfilesModel);
    refreshStationProfileCombo();

    QSqlTableModel* modeModel = new QSqlTableModel();
    modeModel->setTable("modes");
    modeModel->setFilter("enabled = true");
    modeModel->setSort(modeModel->fieldIndex("name"), Qt::AscendingOrder);
    ui->modeEdit->setModel(modeModel);
    ui->modeEdit->setModelColumn(modeModel->fieldIndex("name"));
    modeModel->select();

    QStringList contestList = Data::instance()->contestList();
    contestList.prepend("");
    QStringListModel* contestModel = new QStringListModel(contestList, this);
    ui->contestEdit->setModel(contestModel);

    QStringList propagationModeList = Data::instance()->propagationModesList();
    propagationModeList.prepend("");
    QStringListModel* propagationModeModel = new QStringListModel(propagationModeList, this);
    ui->propagationModeEdit->setModel(propagationModeModel);

    QStringList satModesList = Data::instance()->satModeList();
    satModesList.prepend("");
    QStringListModel* satModesModel = new QStringListModel(satModesList, this);
    ui->satModeEdit->setModel(satModesModel);

    QSqlTableModel* satModel = new QSqlTableModel();
    satModel->setTable("sat_info");

    satCompleter = new QCompleter();
    satCompleter->setModel(satModel);
    satCompleter->setCompletionColumn(satModel->fieldIndex("name"));
    satCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    //satCompleter->setFilterMode(Qt::MatchContains);
    ui->satNameEdit->setCompleter(satCompleter);
    satModel->select();

    ui->satModeEdit->setEnabled(false);
    ui->satNameEdit->setEnabled(false);

    iotaCompleter = new QCompleter(Data::instance()->iotaIDList(), this);
    iotaCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    iotaCompleter->setFilterMode(Qt::MatchContains);
    iotaCompleter->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    ui->iotaEdit->setCompleter(iotaCompleter);

    sotaCompleter = new QCompleter(Data::instance()->sotaIDList(), this);
    sotaCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    sotaCompleter->setFilterMode(Qt::MatchStartsWith);
    sotaCompleter->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    ui->sotaEdit->setCompleter(nullptr);

    connect(rig, &Rig::frequencyChanged,
            this, &NewContactWidget::changeFrequency);

    connect(rig, &Rig::modeChanged,
            this, &NewContactWidget::changeMode);

    connect(rig, &Rig::powerChanged,
            this, &NewContactWidget::changePower);

    contactTimer = new QTimer(this);
    connect(contactTimer, &QTimer::timeout, this, &NewContactWidget::updateTimeOff);

    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(resetContact()), nullptr, Qt::ApplicationShortcut);
    new QShortcut(QKeySequence(Qt::ALT + Qt::Key_W), this, SLOT(resetContact()), nullptr, Qt::ApplicationShortcut);
    new QShortcut(QKeySequence(Qt::Key_F10), this, SLOT(saveContact()), nullptr, Qt::ApplicationShortcut);
    new QShortcut(QKeySequence(Qt::Key_F9), this, SLOT(stopContactTimer()), nullptr, Qt::ApplicationShortcut);
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_M), this, SLOT(markContact()), nullptr, Qt::ApplicationShortcut);

    /*
     * qlog is not a contest log. There is missing many contest features so that it can compete at least a little
     * with the contest logs . Therefore, for now, we will deactivate the tab with the contest information.
     * Maybe later
     * */
    ui->tabWidget_2->removeTab(4);

    ui->frequencyRXEdit->setVisible(false);
    ui->bandRXText->setVisible(false);
    ui->frequencyRXLabel->setVisible(false);
    ui->frequencyTXLabel->setVisible(false);

    reloadSettings();
    readSettings();
    resetContact();
}

void NewContactWidget::readSettings() {
    FCT_IDENTIFICATION;

    QSettings settings;
    QString mode = settings.value("newcontact/mode", "CW").toString();
    QString submode = settings.value("newcontact/submode").toString();
    double realRigFreq = settings.value("newcontact/frequency", 3.5).toDouble();
    double rigFreqOffset = settings.value("newcontact/freqOffset", 0.0).toDouble();
    double rigFreqRXOffset = settings.value("newcontact/freqRXOffset", 0.0).toDouble();
    QString rig = settings.value("newcontact/rig").toString();
    QString ant = settings.value("newcontact/antenna").toString();
    double power = settings.value("newcontact/power", 100).toDouble();
    //QString currStationProfile = StationProfilesManager::instance()->getCurrent().profileName;

    ui->modeEdit->setCurrentText(mode);
    ui->submodeEdit->setCurrentText(submode);
    ui->rigFreqOffsetSpin->setValue(rigFreqOffset);
    ui->rigFreqRXOffsetSpin->setValue(rigFreqRXOffset);
    ui->frequencyEdit->setValue(realRigFreq + ui->rigFreqOffsetSpin->value());
    ui->frequencyRXEdit->setValue(realRigFreq + ui->rigFreqRXOffsetSpin->value());
    ui->rigEdit->setCurrentText(rig);
    ui->antennaEdit->setCurrentText(ant);
    ui->powerEdit->setValue(power);
    //ui->stationProfileCombo->setCurrentText(currStationProfile);

    refreshStationProfileCombo();

}

void NewContactWidget::writeSettings() {
    FCT_IDENTIFICATION;

    QSettings settings;
    settings.setValue("newcontact/mode", ui->modeEdit->currentText());
    settings.setValue("newcontact/submode", ui->submodeEdit->currentText());
    settings.setValue("newcontact/frequency", realRigFreq);
    settings.setValue("newcontact/freqOffset", ui->rigFreqOffsetSpin->value());
    settings.setValue("newcontact/freqRXOffset", ui->rigFreqRXOffsetSpin->value());
    settings.setValue("newcontact/rig", ui->rigEdit->currentText());
    settings.setValue("newcontact/antenna", ui->antennaEdit->currentText());
    settings.setValue("newcontact/power", ui->powerEdit->value());
}

void NewContactWidget::reloadSettings() {
    FCT_IDENTIFICATION;

    QSettings settings;

    QString selectedRig = ui->rigEdit->currentText();
    QString selectedAnt = ui->antennaEdit->currentText();

    QStringList rigs = settings.value("station/rigs").toStringList();
    QStringList ants = settings.value("station/antennas").toStringList();

    QStringListModel* model = dynamic_cast<QStringListModel*>(ui->rigEdit->model());
    QStringListModel* modelAnt = dynamic_cast<QStringListModel*>(ui->antennaEdit->model());

    model->setStringList(rigs);
    modelAnt->setStringList(ants);

    if (!selectedRig.isEmpty()) {
        ui->rigEdit->setCurrentText(selectedRig);
    }

    if (!selectedAnt.isEmpty()) {
        ui->antennaEdit->setCurrentText(selectedAnt);
    }

    //refresh mode combobox
    QString current_mode = ui->modeEdit->currentText();
    ui->modeEdit->clear();
    dynamic_cast<QSqlTableModel*>(ui->modeEdit->model())->select();

    // return selected mode.
    ui->modeEdit->setCurrentText(current_mode);

    /* Reload Callbooks */
    if ( primaryCallbook )
    {
        primaryCallbook->deleteLater();
        primaryCallbook = nullptr;
    }

    if ( secondaryCallbook )
    {
        secondaryCallbook->deleteLater();
        secondaryCallbook = nullptr;
    }

    QString primaryCallbookSelection = settings.value(GenericCallbook::CONFIG_PRIMARY_CALLBOOK_KEY).toString();
    QString secondaryCallbookSelection = settings.value(GenericCallbook::CONFIG_SECONDARY_CALLBOOK_KEY).toString();

    primaryCallbook = createCallbook(primaryCallbookSelection);
    secondaryCallbook = createCallbook(secondaryCallbookSelection);

    if ( primaryCallbook && secondaryCallbook )
    {
        connect(primaryCallbook, &GenericCallbook::callsignNotFound, this, &NewContactWidget::callbookCallsignNotFound);
    }

    /* Refresh Station Profile Combobox */
    refreshStationProfileCombo();
}

void NewContactWidget::callsignChanged() {
    FCT_IDENTIFICATION;

    QString newCallsign = ui->callsignEdit->text().toUpper();
    if (newCallsign == callsign) {
        return;
    }
    else {
        callsign = newCallsign;
    }

    updateTime();
    clearQueryFields();

    if (callsign.isEmpty()) {
        stopContactTimer();
    }
    else {
        startContactTimer();
        queryDxcc(callsign);

        if (callsign.length() >= 3) {
            queryDatabase(callsign);
        }
    }
}

void NewContactWidget::queryDxcc(QString callsign) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<callsign;

    dxccEntity = Data::instance()->lookupDxcc(callsign);
    if (dxccEntity.dxcc) {
        ui->dxccInfo->setText(dxccEntity.country);
        ui->cqEdit->setText(QString::number(dxccEntity.cqz));
        ui->ituEdit->setText(QString::number(dxccEntity.ituz));
        updateCoordinates(dxccEntity.latlon[0], dxccEntity.latlon[1], COORD_DXCC);
        ui->dxccTableWidget->setDxcc(dxccEntity.dxcc);
        ui->contEdit->setCurrentText(dxccEntity.cont);
        updateDxccStatus();
        if (!dxccEntity.flag.isEmpty()) {
            QPixmap flag(QString(":/flags/64/%1.png").arg(dxccEntity.flag));
            ui->flagView->setPixmap(flag);
        }
        else {
            ui->flagView->setPixmap(QPixmap());
        }
    }
    else {
        ui->flagView->setPixmap(QPixmap());
        ui->dxccTableWidget->clear();
        ui->dxccStatus->clear();
        ui->distanceInfo->clear();
        ui->bearingInfo->clear();
        ui->dxccInfo->clear();
        ui->cqEdit->clear();
        ui->ituEdit->clear();
        ui->contEdit->setCurrentText("");
        emit newTarget(0, 0);
    }
}

void NewContactWidget::clearQueryFields()
{
    ui->nameEdit->clear();
    ui->gridEdit->clear();
    ui->qthEdit->clear();
    ui->dokEdit->clear();
    ui->iotaEdit->clear();
    ui->emailEdit->clear();
    ui->countyEdit->clear();
    ui->qslViaEdit->clear();
    ui->urlEdit->clear();
    ui->stateEdit->clear();
}

void NewContactWidget::queryDatabase(QString callsign) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<callsign;

    QSqlQuery query;
    if ( !query.prepare("SELECT name, qth, gridsquare FROM contacts "
                  "WHERE callsign = :callsign ORDER BY start_time DESC LIMIT 1") )
    {
        qWarning() << "Cannot prepare select statement";
        return;
    }
    query.bindValue(":callsign", callsign);

    if ( !query.exec() )
    {
        qWarning() << "Cannot execute statement" << query.lastError();
    }

    if (query.next()){
        ui->nameEdit->setText(query.value(0).toString());
        ui->qthEdit->setText(query.value(1).toString());
        ui->gridEdit->setText(query.value(2).toString());
        emit filterCallsign(callsign);
    }
    else {
        emit filterCallsign(QString());
    }
}

void NewContactWidget::callsignResult(const QMap<QString, QString>& data) {
    FCT_IDENTIFICATION;

    if (data.value("call") != callsign)  {
        return;
    }

    /* not filled or not fully filled then update it */

    if ( ui->nameEdit->text().isEmpty() )
    {
        QString name = data.value("name");

        if ( name.isEmpty() )
        {
            name = data.value("fname");
        }

        if ( ui->nameEdit->text().isEmpty() )
        {
            ui->nameEdit->setText(name);
        }
    }

    if ( ui->gridEdit->text().isEmpty()
         || data.value("gridsquare").contains(ui->gridEdit->text()) )
    {
        ui->gridEdit->setText(data.value("gridsquare"));
    }

    if ( ui->qthEdit->text().isEmpty() )
    {
        ui->qthEdit->setText(data.value("qth"));
    }

    if ( ui->dokEdit->text().isEmpty() )
    {
        ui->dokEdit->setText(data.value("dok"));
    }

    if ( ui->iotaEdit->text().isEmpty() )
    {
        ui->iotaEdit->setText(data.value("iota"));
    }

    if ( ui->emailEdit->text().isEmpty() )
    {
        ui->emailEdit->setText(data.value("email"));
    }

    if ( ui->countyEdit->text().isEmpty() )
    {
        ui->countyEdit->setText(data.value("county"));
    }

    if ( ui->qslViaEdit->text().isEmpty() )
    {
        ui->qslViaEdit->setText(data.value("qsl_via"));
    }

    if ( ui->urlEdit->text().isEmpty() )
    {
        ui->urlEdit->setText(data.value("url"));
    }

    if ( ui->stateEdit->text().isEmpty() )
    {
        ui->stateEdit->setText(data.value("us_state"));
    }
}

/* call when newcontact frequency spinbox is changed */
void NewContactWidget::frequencyChanged()
{
    FCT_IDENTIFICATION;

    realRigFreq = ui->frequencyEdit->value() - ui->rigFreqOffsetSpin->value();

    ui->frequencyRXEdit->blockSignals(true);
    ui->frequencyRXEdit->setValue(realRigFreq + ui->rigFreqRXOffsetSpin->value());
    updateRXBand(realRigFreq + ui->rigFreqRXOffsetSpin->value());
    ui->frequencyRXEdit->blockSignals(false);

    updateBand(ui->frequencyEdit->value()); // show a converted frequency
    rig->setFrequency(realRigFreq);  // set rig frequency
    qCDebug(runtime) << "rig real freq: " << realRigFreq;
    emit userFrequencyChanged(ui->frequencyEdit->value());
}

void NewContactWidget::frequencyRXChanged()
{
    FCT_IDENTIFICATION;

    realRigFreq = ui->frequencyRXEdit->value() - ui->rigFreqRXOffsetSpin->value();

    ui->frequencyEdit->blockSignals(true);
    ui->frequencyEdit->setValue(realRigFreq + ui->rigFreqOffsetSpin->value());
    updateBand(realRigFreq + ui->rigFreqRXOffsetSpin->value());
    ui->frequencyEdit->blockSignals(false);

    updateRXBand(ui->frequencyEdit->value()); // show a converted frequency
    rig->setFrequency(realRigFreq);  // set rig frequency
    qCDebug(runtime) << "rig real freq: " << realRigFreq;
    emit userFrequencyChanged(ui->frequencyEdit->value()); // emit only TX frequency here because qlog tune based on
}

void NewContactWidget::bandChanged() {
    FCT_IDENTIFICATION;

    updateDxccStatus();
}

void NewContactWidget::__modeChanged()
{
    FCT_IDENTIFICATION;

    QSqlTableModel* modeModel = dynamic_cast<QSqlTableModel*>(ui->modeEdit->model());
    QSqlRecord record = modeModel->record(ui->modeEdit->currentIndex());
    QString submodes = record.value("submodes").toString();

    QStringList submodeList = QJsonDocument::fromJson(submodes.toUtf8()).toVariant().toStringList();
    QStringListModel* model = dynamic_cast<QStringListModel*>(ui->submodeEdit->model());
    model->setStringList(submodeList);

    if (!submodeList.isEmpty()) {
        submodeList.prepend("");
        model->setStringList(submodeList);
        ui->submodeEdit->setEnabled(true);
        ui->submodeEdit->setCurrentIndex(1);
    }
    else {
        QStringList list;
        model->setStringList(list);
        ui->submodeEdit->setEnabled(false);
        ui->submodeEdit->setCurrentIndex(-1);
    }

    defaultReport = record.value("rprt").toString();

    setDefaultReport();
    updateDxccStatus();

}

void NewContactWidget::refreshStationProfileCombo()
{
    FCT_IDENTIFICATION;

    ui->stationProfileCombo->blockSignals(true);

    QStringList currProfiles = StationProfilesManager::instance()->profilesList();
    QStringListModel* model = dynamic_cast<QStringListModel*>(ui->stationProfileCombo->model());

    model->setStringList(currProfiles);

    if ( StationProfilesManager::instance()->getCurrent().profileName.isEmpty()
         && currProfiles.count() > 0 )
    {
        /* changing profile from empty to something */
        ui->stationProfileCombo->setCurrentText(currProfiles.first());
        stationProfileChanged(currProfiles.first());
    }
    else
    {
        /* no profile change, just refresh the combo and preserve current profile */
        ui->stationProfileCombo->setCurrentText(StationProfilesManager::instance()->getCurrent().profileName);
    }

    ui->stationProfileCombo->blockSignals(false);
}

/* Mode is changed from GUI */
void NewContactWidget::modeChanged()
{
    FCT_IDENTIFICATION;

    ui->submodeEdit->blockSignals(true);
    __modeChanged();
    ui->submodeEdit->blockSignals(false);
    rig->setMode(ui->modeEdit->currentText(), ui->submodeEdit->currentText());

}

void NewContactWidget::subModeChanged()
{
    FCT_IDENTIFICATION;

    rig->setMode(ui->modeEdit->currentText(), ui->submodeEdit->currentText());
}

void NewContactWidget::updateBand(double freq) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<freq;

    Band band = Data::band(freq);

    if (band.name.isEmpty()) {
        ui->bandText->setText("OOB!");
    }
    else if (band.name != ui->bandText->text()) {
        ui->bandText->setText(band.name);
        bandChanged();
    }
}

void NewContactWidget::updateRXBand(double freq)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<freq;

    Band band = Data::band(freq);

    if (band.name.isEmpty()) {
        ui->bandRXText->setText("OOB!");
    }
    else if (band.name != ui->bandRXText->text()) {
        ui->bandRXText->setText(band.name);
    }

}

void NewContactWidget::gridChanged() {
    FCT_IDENTIFICATION;

    Gridsquare newGrid(ui->gridEdit->text());

    if (!newGrid.isValid())
    {
        coordPrec = COORD_NONE;
        queryDxcc(ui->callsignEdit->text().toUpper());
        return;
    }

    updateCoordinates(newGrid.getLatitude(), newGrid.getLongitude(), COORD_GRID);
}

void NewContactWidget::resetContact() {
    FCT_IDENTIFICATION;

    updateTime();
    ui->callsignEdit->clear();
    ui->nameEdit->clear();
    ui->qthEdit->clear();
    ui->gridEdit->clear();
    ui->commentEdit->clear();
    ui->noteEdit->clear();
    ui->dxccInfo->clear();
    ui->distanceInfo->clear();
    ui->bearingInfo->clear();
    ui->qslViaEdit->clear();
    ui->qslSentBox->setCurrentIndex(0);
    ui->qslSentViaBox->setCurrentIndex(0);
    ui->cqEdit->clear();
    ui->ituEdit->clear();
    ui->contEdit->setCurrentText("");
    ui->countyEdit->clear();
    ui->stateEdit->clear();
    ui->iotaEdit->clear();
    ui->sotaEdit->clear();
    ui->sigEdit->clear();
    ui->sigInfoEdit->clear();
    ui->dokEdit->clear();
    ui->vuccEdit->clear();
    ui->dxccTableWidget->clear();
    ui->dxccStatus->clear();
    ui->flagView->setPixmap(QPixmap());
    ui->ageEdit->clear();
    ui->emailEdit->clear();
    ui->urlEdit->clear();

    stopContactTimer();
    setDefaultReport();

    ui->callsignEdit->setPalette(QPalette());
    ui->callsignEdit->setFocus();
    callsign = QString();
    coordPrec = COORD_NONE;

    emit filterCallsign(QString());
    emit newTarget(0, 0);
}

void NewContactWidget::addAddlFields(QSqlRecord &record)
{
    FCT_IDENTIFICATION;

    StationProfile profile = StationProfilesManager::instance()->getCurrent();

    record.setValue("qsl_sent", "N");
    record.setValue("qsl_rcvd", "N");
    record.setValue("lotw_qsl_sent", "N");
    record.setValue("lotw_qsl_rcvd", "N");
    record.setValue("eqsl_qsl_rcvd", "N");
    record.setValue("eqsl_qsl_sent", "N");
    record.setValue("hrdlog_qso_upload_status", "N");
    record.setValue("qrzcom_qsoupload_status", "N");

    /* isNull is not necessary to use because NULL Text fields are empty */
    if ( record.value("my_gridsquare").toString().isEmpty()
         && !profile.locator.isEmpty())
    {
        record.setValue("my_gridsquare", profile.locator.toUpper());
    }

    if ( ! record.value("gridsquare").toString().isEmpty()
         && ! record.value("my_gridsquare").toString().isEmpty() )
    {
        Gridsquare myGrid(record.value("my_gridsquare").toString());
        double distance;
        if ( myGrid.distanceTo(Gridsquare(record.value("gridsquare").toString()), distance) )
        {
            record.setValue("distance", distance);
        }
    }

    if ( prop_cond )
    {
        if ( record.value("sfi").isNull()
             && prop_cond->isFluxValid() )
        {
            record.setValue("sfi", prop_cond->getFlux());
        }

        if ( record.value("k_index").isNull()
             && prop_cond->isKIndexValid() )
        {
            record.setValue("k_index", prop_cond->getKIndex());
        }

        if ( record.value("a_index").isNull()
             && prop_cond->isAIndexValid() )
        {
            record.setValue("a_index", prop_cond->getAIndex());
        }
    }

    if ( (record.value("tx_pwr").isNull() || record.value("tx_pwr") == 0.0 )
         && ui->powerEdit->value() != 0.0)
    {
        record.setValue("tx_pwr", ui->powerEdit->value());
    }

    if ( record.value("band").toString().isEmpty()
         && ! record.value("freq").isNull() )
    {
        record.setValue("band", Data::freqToBand(record.value("freq").toDouble()));
    }

    if ( record.value("prop_mode").toString().isEmpty()
         && !ui->propagationModeEdit->currentText().isEmpty())
    {
        record.setValue("prop_mode", Data::instance()->propagationModeTextToID(ui->propagationModeEdit->currentText()));
    }

    if ( record.value("sat_mode").toString().isEmpty()
         && !ui->satModeEdit->currentText().isEmpty() )
    {
        record.setValue("sat_mode", Data::instance()->satModeTextToID(ui->satModeEdit->currentText()));
    }

    if ( record.value("sat_name").toString().isEmpty()
         && !ui->satNameEdit->text().isEmpty() )
    {
        record.setValue("sat_name", ui->satNameEdit->text().toUpper());
    }

    if ( record.value("my_rig").toString().isEmpty()
         && !ui->rigEdit->currentText().isEmpty() )
    {
       record.setValue("my_rig", Data::removeAccents(ui->rigEdit->currentText()));
    }

    if ( record.value("my_rig_intl").toString().isEmpty()
         && !ui->rigEdit->currentText().isEmpty() )
    {
       record.setValue("my_rig_intl", ui->rigEdit->currentText());
    }

    if ( record.value("my_antenna").toString().isEmpty()
         && !ui->antennaEdit->currentText().isEmpty())
    {
       record.setValue("my_antenna", Data::removeAccents(ui->antennaEdit->currentText()));
    }

    if ( record.value("my_antenna_intl").toString().isEmpty()
         && !ui->antennaEdit->currentText().isEmpty())
    {
       record.setValue("my_antenna_intl", ui->antennaEdit->currentText());
    }

    if ( record.value("my_city").toString().isEmpty()
         && !profile.qthName.isEmpty())
    {
       record.setValue("my_city", Data::removeAccents(profile.qthName.toUpper()));
    }

    if ( record.value("my_city_intl").toString().isEmpty()
         && !profile.qthName.isEmpty())
    {
       record.setValue("my_city_intl", profile.qthName.toUpper());
    }

    if ( record.value("station_callsign").toString().isEmpty()
         && !profile.callsign.isEmpty() )
    {
       record.setValue("station_callsign", profile.callsign.toUpper());
    }

    if ( record.value("operator").toString().isEmpty()
         && !profile.operatorName.isEmpty() )
    {
       record.setValue("operator", Data::removeAccents(profile.operatorName.toUpper()));
    }

    if ( record.value("my_iota").toString().isEmpty()
         && !profile.iota.isEmpty())
    {
       record.setValue("my_iota", profile.iota.toUpper());
    }

    if ( record.value("my_sota_ref").toString().isEmpty()
         && !profile.sota.isEmpty())
    {
       record.setValue("my_sota_ref", profile.sota.toUpper());
    }

    if ( record.value("my_sig").toString().isEmpty()
         && !profile.sig.isEmpty())
    {
       record.setValue("my_sig", Data::removeAccents(profile.sig.toUpper()));
    }

    if ( record.value("my_sig_intl").toString().isEmpty()
         && !profile.sig.isEmpty())
    {
       record.setValue("my_sig_intl", profile.sig.toUpper());
    }

    if ( record.value("my_sig_info").toString().isEmpty()
         && !profile.sigInfo.isEmpty())
    {
       record.setValue("my_sig_info", Data::removeAccents(profile.sigInfo.toUpper()));
    }

    if ( record.value("my_sig_info_intl").toString().isEmpty()
         && !profile.sigInfo.isEmpty())
    {
       record.setValue("my_sig_info_intl", profile.sigInfo.toUpper());
    }

    if ( record.value("my_vucc_grids").toString().isEmpty()
         && !profile.vucc.isEmpty())
    {
       record.setValue("my_vucc_grids", profile.vucc.toUpper());
    }
}

GenericCallbook *NewContactWidget::createCallbook(const QString &callbookID)
{
    FCT_IDENTIFICATION;

    GenericCallbook *ret = nullptr;
    QString callbookString;

    if (callbookID == HamQTH::CALLBOOK_NAME )
    {
        ret = new HamQTH(this);
        callbookString = tr("HamQTH");
    }
    else if ( callbookID == QRZ::CALLBOOK_NAME )
    {
        ret = new QRZ(this);
        callbookString = tr("QRZ.com");
    }

    if ( ret )
    {
        connect(ret, &GenericCallbook::callsignResult, this, &NewContactWidget::callsignResult);
        connect(ret, &GenericCallbook::loginFailed, this, [this, callbookString]()
        {
            QMessageBox::critical(this, tr("QLog Error"), callbookString + " " + tr("Callbook login failed"));
        });
    }

    return ret;
}
void NewContactWidget::saveContact()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    StationProfile profile = StationProfilesManager::instance()->getCurrent();

    if ( profile.callsign.isEmpty() )
    {
        QMessageBox::critical(nullptr, QMessageBox::tr("QLog Error"),
                              QMessageBox::tr("Your callsign is empty. Please, set your Station Profile"));
        return;
    }

    QSqlTableModel model;
    model.setTable("contacts");
    model.removeColumn(model.fieldIndex("id"));

    QDateTime start = QDateTime(ui->dateEdit->date(), ui->timeOnEdit->time(), Qt::UTC);
    QDateTime end = QDateTime(ui->dateEdit->date(), ui->timeOffEdit->time(), Qt::UTC);

    QSqlRecord record = model.record();

    record.setValue("start_time", start);
    record.setValue("end_time", end);

    if ( ! callsign.isEmpty() )
    {
        record.setValue("callsign", callsign);
    }

    if ( ! ui->rstSentEdit->text().isEmpty() )
    {
        record.setValue("rst_sent", ui->rstSentEdit->text());
    }

    if ( ! ui->rstRcvdEdit->text().isEmpty() )
    {
        record.setValue("rst_rcvd", ui->rstRcvdEdit->text());
    }

    if ( ! ui->nameEdit->text().isEmpty() )
    {
        record.setValue("name_intl", ui->nameEdit->text());
        record.setValue("name", Data::removeAccents(ui->nameEdit->text()));
    }

    if ( ! ui->qthEdit->text().isEmpty() )
    {
        record.setValue("qth_intl", ui->qthEdit->text());
        record.setValue("qth", Data::removeAccents(ui->qthEdit->text()));
    }

    if ( ! ui->gridEdit->text().isEmpty() )
    {
        record.setValue("gridsquare", ui->gridEdit->text().toUpper());
    }

    qInfo()<<""<<ui->frequencyEdit->value();

    record.setValue("freq", ui->frequencyEdit->value());
    record.setValue("band", ui->bandText->text());

    if ( ui->rigFreqRXOffsetSpin->value() != 0 )
    {
        record.setValue("freq_rx", ui->frequencyRXEdit->value());
        record.setValue("band_rx", ui->bandRXText->text());
    }
    else
    {
        record.setValue("freq_rx", ui->frequencyEdit->value());
        record.setValue("band_rx", ui->bandText->text());
    }

    if ( ! ui->modeEdit->currentText().isEmpty() )
    {
        record.setValue("mode", ui->modeEdit->currentText());
    }

    if ( ! ui->submodeEdit->currentText().isEmpty() )
    {
        record.setValue("submode", ui->submodeEdit->currentText());
    }

    record.setValue("cqz", ui->cqEdit->text().toInt());
    record.setValue("ituz", ui->ituEdit->text().toInt());
    record.setValue("dxcc", dxccEntity.dxcc);
    record.setValue("country", dxccEntity.country);

    if ( ! ui->contEdit->currentText().isEmpty() )
    {
        record.setValue("cont", ui->contEdit->currentText());
    }

    if ( ! ui->countyEdit->text().isEmpty() )
    {
        record.setValue("cnty", ui->countyEdit->text());
    }

    if ( ! ui->stateEdit->text().isEmpty() )
    {
        record.setValue("state", Data::removeAccents(ui->stateEdit->text()));
    }

    if ( ! ui->iotaEdit->text().isEmpty() )
    {
        record.setValue("iota", ui->iotaEdit->text().toUpper());
    }

    if ( ! ui->sigEdit->text().isEmpty() )
    {
        record.setValue("sig", Data::removeAccents(ui->sigEdit->text().toUpper()));
        record.setValue("sig_intl", ui->sigEdit->text().toUpper());
    }

    if ( ! ui->sigInfoEdit->text().isEmpty() )
    {
        record.setValue("sig_info", Data::removeAccents(ui->sigInfoEdit->text().toUpper()));
        record.setValue("sig_info_intl", ui->sigInfoEdit->text().toUpper());
    }

    record.setValue("qsl_sent", ui->qslSentBox->itemData(ui->qslSentBox->currentIndex()));

    if ( ! ui->qslSentViaBox->currentText().isEmpty() )
    {
        record.setValue("qsl_sent_via", ui->qslSentViaBox->itemData(ui->qslSentViaBox->currentIndex()));
    }

    if ( coordPrec >= COORD_GRID)
    {
        record.setValue("distance", ui->distanceInfo->text().split(" ")[0]);
    }

    if ( !ui->sotaEdit->text().isEmpty() )
    {
        record.setValue("sota_ref", ui->sotaEdit->text().toUpper());
    }

    if ( !ui->dokEdit->text().isEmpty() )
    {
        record.setValue("darc_dok", ui->dokEdit->text().toUpper());
    }

    if ( !ui->vuccEdit->text().isEmpty() )
    {
        record.setValue("vucc_grids", ui->vuccEdit->text().toUpper());
    }

    if (!ui->commentEdit->text().isEmpty()) {
        record.setValue("comment_intl", ui->commentEdit->text());
        record.setValue("comment", Data::removeAccents(ui->commentEdit->text()));
    }

    if (!ui->noteEdit->toPlainText().isEmpty())
    {
        record.setValue("notes_intl", ui->noteEdit->toPlainText());
    }

    if (!ui->qslViaEdit->text().isEmpty()) {
        record.setValue("qsl_via", ui->qslViaEdit->text().toUpper());
    }

    if (!ui->ageEdit->text().isEmpty()
        && ui->ageEdit->text().toInt() > 0 )
    {
        record.setValue("age", ui->ageEdit->text().toInt());
    }

    if (!ui->emailEdit->text().isEmpty()) {
        record.setValue("email", ui->emailEdit->text());
    }

    if (!ui->urlEdit->text().isEmpty()) {
        record.setValue("web", ui->urlEdit->text());
    }    

    addAddlFields(record);

    qCDebug(runtime) << record;

    if ( !model.insertRecord(-1, record) )
    {
        qCDebug(runtime) << model.lastError();
        return;
    }

    if ( !model.submitAll() )
    {
        qCDebug(runtime) << model.lastError();
        return;
    }

    resetContact();

    emit contactAdded(record);
}

void NewContactWidget::saveExternalContact(QSqlRecord record)
{
    FCT_IDENTIFICATION;

    if ( record.value("callsign").toString().isEmpty() ) return;

    QSqlTableModel model;

    model.setTable("contacts");
    model.removeColumn(model.fieldIndex("id"));

    DxccEntity dxcc = Data::instance()->lookupDxcc(record.value("callsign").toString());

    if ( !dxcc.country.isEmpty() )
    {
        record.setValue("country", dxcc.country);
        record.setValue("cqz", dxcc.cqz);
        record.setValue("ituz", dxcc.ituz);
        record.setValue("cont", dxcc.cont);
        record.setValue("dxcc", dxcc.dxcc);
    }

    addAddlFields(record);

    qCDebug(runtime) << record;

    if ( !model.insertRecord(-1, record) )
    {
        qCInfo(runtime) << model.lastError();
        return;
    }

    if ( !model.submitAll() )
    {
        qCInfo(runtime) << model.lastError();
        return;
    }

    emit contactAdded(record);
}

void NewContactWidget::startContactTimer() {
    FCT_IDENTIFICATION;

    updateTime();
    if (!contactTimer->isActive()) {
        contactTimer->start(1000);
    }
}

void NewContactWidget::stopContactTimer() {
    FCT_IDENTIFICATION;

    if (contactTimer->isActive()) {
        contactTimer->stop();
    }
    updateTimeOff();
}

void NewContactWidget::markContact()
{
    FCT_IDENTIFICATION;

    if ( !ui->callsignEdit->text().isEmpty() )
    {
        DxSpot spot;

        spot.time = QDateTime::currentDateTimeUtc().time(); //QTime::currentTime();
        spot.freq = ui->frequencyEdit->value();
        spot.band = Data::band(spot.freq).name;
        spot.callsign = ui->callsignEdit->text().toUpper();
        emit markQSO(spot);
    }
}

void NewContactWidget::editCallsignFinished()
{
    startContactTimer();
    if ( callsign.size() >= 3 && primaryCallbook )
    {
        primaryCallbook->queryCallsign(callsign);
    }
}

void NewContactWidget::updateTime() {
    FCT_IDENTIFICATION;

    QDateTime now = QDateTime::currentDateTimeUtc();
    ui->dateEdit->setDate(now.date());
    ui->timeOnEdit->setTime(now.time());
    ui->timeOffEdit->setTime(now.time());
}

void NewContactWidget::updateTimeStop() {
    FCT_IDENTIFICATION;

    updateTime();
    stopContactTimer();
}

void NewContactWidget::updateTimeOff() {
    FCT_IDENTIFICATION;

    ui->timeOffEdit->setTime(QDateTime::currentDateTimeUtc().time());
}

void NewContactWidget::updateCoordinates(double lat, double lon, CoordPrecision prec) {
    FCT_IDENTIFICATION;

    if (prec < coordPrec) return;

    Gridsquare myGrid(StationProfilesManager::instance()->getCurrent().locator);
    double distance;
    double bearing;

    if ( myGrid.distanceTo(lat, lon, distance)
         && myGrid.bearingTo(lat, lon, bearing) )
    {
        ui->distanceInfo->setText(QString::number(distance, '.', 1) + " km");
        ui->bearingInfo->setText(QString("%1°").arg(bearing));

        coordPrec = prec;

        emit newTarget(lat, lon);
    }

}

void NewContactWidget::updateDxccStatus() {
    FCT_IDENTIFICATION;

    if (callsign.isEmpty()) {
        ui->dxccStatus->clear();
        ui->callsignEdit->setPalette(QPalette());
        return;
    }

    DxccStatus status = Data::dxccStatus(dxccEntity.dxcc, ui->bandText->text(), ui->modeEdit->currentText());
    switch (status) {
    case DxccStatus::NewEntity:
        ui->dxccStatus->setText(tr("New Entity!"));
        break;
    case DxccStatus::NewBand:
        ui->dxccStatus->setText(tr("New Band!"));
        break;
    case DxccStatus::NewMode:
        ui->dxccStatus->setText(tr("New Mode!"));
        break;
    case DxccStatus::NewBandMode:
        ui->dxccStatus->setText(tr("New Band & Mode!"));
        break;
    case DxccStatus::NewSlot:
        ui->dxccStatus->setText(tr("New Slot!"));
        break;
    default:
        ui->dxccStatus->clear();
    }

    QPalette palette;
    palette.setColor(QPalette::Text, Data::statusToColor(status, palette.color(QPalette::Text)));
    ui->callsignEdit->setPalette(palette);
}

/* call when rig freq is changed */
void NewContactWidget::changeFrequency(double freq) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<freq;
    qCDebug(runtime) << "current freq offset " << ui->rigFreqOffsetSpin->value();
    realRigFreq = freq;

    ui->frequencyEdit->blockSignals(true);
    ui->frequencyEdit->setValue(realRigFreq + ui->rigFreqOffsetSpin->value());
    updateBand(realRigFreq + ui->rigFreqOffsetSpin->value());
    ui->frequencyEdit->blockSignals(false);

    ui->frequencyRXEdit->blockSignals(true);
    ui->frequencyRXEdit->setValue(realRigFreq + ui->rigFreqRXOffsetSpin->value());
    updateRXBand(realRigFreq + ui->rigFreqRXOffsetSpin->value());
    ui->frequencyRXEdit->blockSignals(false);


}

/* mode is changed from RIG */
void NewContactWidget::changeMode(QString mode, QString subMode) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<mode<< " " << subMode;

    ui->modeEdit->blockSignals(true);
    ui->submodeEdit->blockSignals(true);
    ui->modeEdit->setCurrentText(mode);
    __modeChanged();
    ui->submodeEdit->setCurrentText(subMode);
    ui->submodeEdit->blockSignals(false);
    ui->modeEdit->blockSignals(false);
}

void NewContactWidget::changePower(double power) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<power;

    ui->powerEdit->blockSignals(true);
    ui->powerEdit->setValue(power);
    ui->powerEdit->blockSignals(false);
}

void NewContactWidget::tuneDx(QString callsign, double frequency) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<callsign<< " " << frequency;
    resetContact();
    ui->callsignEdit->setText(callsign);
    ui->frequencyEdit->setValue(frequency);
    callsignChanged();
    if ( callsign.size() >= 3 && primaryCallbook )
    {
        primaryCallbook->queryCallsign(callsign);
    }
    stopContactTimer();
}

void NewContactWidget::showDx(QString callsign, QString grid)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<callsign<< " " << grid;

    resetContact();
    ui->callsignEdit->setText(callsign.toUpper());
    ui->gridEdit->setText(grid);
    callsignChanged();
    if ( callsign.size() >= 3  && primaryCallbook )
    {
        primaryCallbook->queryCallsign(callsign);
    }
    stopContactTimer();
}

void NewContactWidget::setDefaultReport() {
    FCT_IDENTIFICATION;

    if (defaultReport.isEmpty()) {
        defaultReport = "599";
    }

    ui->rstRcvdEdit->setText(defaultReport);
    ui->rstSentEdit->setText(defaultReport);
}

void NewContactWidget::qrz() {
    FCT_IDENTIFICATION;

    QDesktopServices::openUrl(QString("https://www.qrz.com/lookup/%1").arg(callsign));
}

void NewContactWidget::addPropConditions(Conditions *cond)
{
    FCT_IDENTIFICATION;
    prop_cond = cond;
}

void NewContactWidget::propModeChanged(const QString &propModeText)
{
    FCT_IDENTIFICATION;

    qCDebug(runtime) << "propModeText: " << propModeText << " mode: "<< Data::instance()->propagationModeIDToText("SAT");
    if ( propModeText == Data::instance()->propagationModeIDToText("SAT") )
    {
        ui->satModeEdit->setEnabled(true);
        ui->satNameEdit->setEnabled(true);
    }
    else
    {
        ui->satModeEdit->setCurrentIndex(-1);
        ui->satNameEdit->clear();
        ui->satModeEdit->setEnabled(false);
        ui->satNameEdit->setEnabled(false);
    }
}

/* call when freq offset is changed */
void NewContactWidget::rigFreqOffsetChanged(double offset)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << offset;

    double new_freq = realRigFreq + offset;
    ui->frequencyEdit->setValue(new_freq);
    updateBand(new_freq);
    qCDebug(runtime) << "rig real freq: " << realRigFreq;
}

void NewContactWidget::rigFreqRXOffsetChanged(double offset)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << offset;

    if ( offset != 0 )
    {
        ui->frequencyRXEdit->setVisible(true);
        ui->bandRXText->setVisible(true);
        ui->frequencyRXLabel->setVisible(true);
        ui->frequencyTXLabel->setVisible(true);
    }
    else
    {
        ui->frequencyRXEdit->setVisible(false);
        ui->bandRXText->setVisible(false);
        ui->frequencyRXLabel->setVisible(false);
        ui->frequencyTXLabel->setVisible(false);
    }

    double new_freq = realRigFreq + offset;
    ui->frequencyRXEdit->setValue(new_freq);
    updateRXBand(new_freq);
    qCDebug(runtime) << "rig real freq: " << realRigFreq;
}

void NewContactWidget::stationProfileChanged(QString profileName)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << profileName;

    StationProfilesManager::instance()->setCurrent(profileName);

    emit newStationProfile();
}

void NewContactWidget::sotaChanged(QString newSOTA)
{
    FCT_IDENTIFICATION;

    if ( newSOTA.length() >= 3 )
    {
        ui->sotaEdit->setCompleter(sotaCompleter);
    }
    else
    {
        ui->sotaEdit->setCompleter(nullptr);
    }
}

void NewContactWidget::callbookCallsignNotFound(QString queryCallsign)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << queryCallsign;

    if ( queryCallsign != callsign )
        return ;

    if (!secondaryCallbook) return;

    secondaryCallbook->queryCallsign(queryCallsign);
}

NewContactWidget::~NewContactWidget() {
    FCT_IDENTIFICATION;

    writeSettings();
    if ( primaryCallbook )
    {
        primaryCallbook->deleteLater();
    }

    if ( secondaryCallbook )
    {
        secondaryCallbook->deleteLater();
    }

    delete ui;
}
