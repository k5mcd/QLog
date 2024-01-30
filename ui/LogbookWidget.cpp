#include <QtSql>
#include <QMessageBox>
#include <QDesktopServices>
#include <QMenu>
#include <QProgressDialog>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QShortcut>

#include "logformat/AdiFormat.h"
#include "models/LogbookModel.h"
#include "models/SqlListModel.h"
#include "core/ClubLog.h"
#include "LogbookWidget.h"
#include "ui_LogbookWidget.h"
#include "ui/StyleItemDelegate.h"
#include "core/debug.h"
#include "models/SqlListModel.h"
#include "ui/ColumnSettingDialog.h"
#include "data/Data.h"
#include "ui/ExportDialog.h"
#include "core/Eqsl.h"
#include "ui/PaperQSLDialog.h"
#include "core/NetworkNotification.h"
#include "ui/QSODetailDialog.h"
#include "core/MembershipQE.h"
#include "core/GenericCallbook.h"

MODULE_IDENTIFICATION("qlog.ui.logbookwidget");

LogbookWidget::LogbookWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogbookWidget)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    model = new LogbookModel(this);
    connect(model, &LogbookModel::beforeUpdate, this, &LogbookWidget::handleBeforeUpdate);
    connect(model, &LogbookModel::beforeDelete, this, &LogbookWidget::handleBeforeDelete);

    ui->contactTable->setModel(model);

    ui->contactTable->addAction(ui->actionEditContact);
    ui->contactTable->addAction(ui->actionFilter);
    ui->contactTable->addAction(ui->actionExportAs);
    ui->contactTable->addAction(ui->actionLookup);
    ui->contactTable->addAction(ui->actionDisplayedColumns);
    //ui->contactTable->addAction(ui->actionUploadClublog);
    ui->contactTable->addAction(ui->actionDeleteContact);

    //ui->contactTable->sortByColumn(1, Qt::DescendingOrder);

    ui->contactTable->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->contactTable->horizontalHeader(), &QHeaderView::customContextMenuRequested,
            this, &LogbookWidget::showTableHeaderContextMenu);
    connect(ui->contactTable, &QTableQSOView::dataCommitted, this, &LogbookWidget::updateTable);

    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_TIME_ON, new TimestampFormatDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_TIME_OFF, new TimestampFormatDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_CALL, new CallsignDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_FREQUENCY, new UnitFormatDelegate("", 6, 0.001, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_BAND, new ComboFormatDelegate(new SqlListModel("SELECT name FROM bands ORDER BY start_freq", " "), ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_MODE, new ComboFormatDelegate(new SqlListModel("SELECT name FROM modes", " "), ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_CONTINENT, new ComboFormatDelegate(QStringList()<<" "<< "AF" << "AN" << "AS" << "EU" << "NA" << "OC" << "SA"));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_QSL_SENT, new ComboFormatDelegate(Data::instance()->qslSentEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_QSL_SENT_VIA, new ComboFormatDelegate(Data::instance()->qslSentViaEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_QSL_RCVD, new ComboFormatDelegate(Data::instance()->qslRcvdEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_QSL_RCVD_VIA, new ComboFormatDelegate(Data::instance()->qslSentViaEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_QSL_SENT_DATE, new DateFormatDelegate());
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_QSL_RCVD_DATE, new DateFormatDelegate());
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_LOTW_SENT, new ComboFormatDelegate(Data::instance()->qslSentEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_LOTW_RCVD, new ComboFormatDelegate(Data::instance()->qslRcvdEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_LOTW_RCVD_DATE, new DateFormatDelegate());
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_LOTW_SENT_DATE, new DateFormatDelegate());
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_TX_POWER, new UnitFormatDelegate("W", 3, 0.1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_AGE, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_ALTITUDE, new UnitFormatDelegate("m", 2, 0.1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_A_INDEX, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_ANT_AZ, new UnitFormatDelegate("", 1, 0.1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_ANT_EL, new UnitFormatDelegate("", 1, 0.1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_ANT_PATH, new ComboFormatDelegate(Data::instance()->antPathEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_CLUBLOG_QSO_UPLOAD_DATE, new DateFormatDelegate());
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_CLUBLOG_QSO_UPLOAD_STATUS, new ComboFormatDelegate(Data::instance()->uploadStatusEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_DISTANCE, new DistanceFormatDelegate(1, 0.1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_EQSL_QSLRDATE, new DateFormatDelegate());
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_EQSL_QSLSDATE, new DateFormatDelegate());
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_EQSL_QSL_RCVD, new ComboFormatDelegate(Data::instance()->qslRcvdEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_EQSL_QSL_SENT, new ComboFormatDelegate(Data::instance()->qslSentEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_FISTS, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_FISTS_CC, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_FORCE_INIT, new ComboFormatDelegate(Data::instance()->boolEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_FREQ_RX, new UnitFormatDelegate("", 6, 0.001, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_BAND_RX, new ComboFormatDelegate(new SqlListModel("SELECT name FROM bands ORDER BY start_freq", " "), ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_HAMLOGEU_QSO_UPLOAD_DATE, new DateFormatDelegate());
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_HAMLOGEU_QSO_UPLOAD_STATUS, new ComboFormatDelegate(Data::instance()->uploadStatusEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_HAMQTH_QSO_UPLOAD_DATE, new DateFormatDelegate());
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_HAMQTH_QSO_UPLOAD_STATUS, new ComboFormatDelegate(Data::instance()->uploadStatusEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_HRDLOG_QSO_UPLOAD_DATE, new DateFormatDelegate());
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_HRDLOG_QSO_UPLOAD_STATUS, new ComboFormatDelegate(Data::instance()->uploadStatusEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_IOTA_ISLAND_ID, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_K_INDEX, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_MAX_BURSTS, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_MY_ALTITUDE, new UnitFormatDelegate("m", 2, 0.1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_MY_CQ_ZONE, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_MY_DXCC, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_MY_FISTS, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_MY_IOTA_ISLAND_ID, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_MY_ITU_ZONE, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_NR_BURSTS, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_NR_PINGS, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_NOTES_INTL, new TextBoxDelegate(this));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_NOTES, new TextBoxDelegate(this));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_PROP_MODE, new ComboFormatDelegate(QStringList()<<" "<< Data::instance()->propagationModesIDList(), ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_QRZCOM_QSO_UPLOAD_DATE, new DateFormatDelegate());
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_QRZCOM_QSO_UPLOAD_STATUS, new ComboFormatDelegate(Data::instance()->uploadStatusEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_QSO_COMPLETE, new ComboFormatDelegate(Data::instance()->qsoCompleteEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_QSO_RANDOM, new ComboFormatDelegate(Data::instance()->boolEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_RX_PWR, new UnitFormatDelegate("W", 3, 0.1, ui->contactTable));
    /*https://www.pe0sat.vgnet.nl/satellite/sat-information/modes/ */
    /* use all possible values, do not use only modern modes in sat_modes.json */
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_SAT_MODE, new ComboFormatDelegate(QStringList()<<" "<<"VU"<<"UV"<<"US"<<"LU"<<"LS"<<"LX"<<"VS"<<"SX"<<"K"<<"T"<<"A"<<"J"<<"B"<<"S"<<"L", ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_SFI, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_SILENT_KEY, new ComboFormatDelegate(Data::instance()->boolEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_SRX, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_STX, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_SWL, new ComboFormatDelegate(Data::instance()->boolEnum, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_TEN_TEN, new UnitFormatDelegate("", 0, 1, ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(LogbookModel::COLUMN_UKSMG, new UnitFormatDelegate("", 0, 1, ui->contactTable));

    QSettings settings;
    QVariant logbookState = settings.value("logbook/state");
    if (!logbookState.isNull()) {
        ui->contactTable->horizontalHeader()->restoreState(logbookState.toByteArray());
    }
    else {
        /* Hide all */
        for ( int i = 0; i < LogbookModel::COLUMN_LAST_ELEMENT; i++ )
        {
            ui->contactTable->hideColumn(i);
        }
        /* Set a basic set of columns */
        ui->contactTable->showColumn(LogbookModel::COLUMN_TIME_ON);
        ui->contactTable->showColumn(LogbookModel::COLUMN_CALL);
        ui->contactTable->showColumn(LogbookModel::COLUMN_RST_RCVD);
        ui->contactTable->showColumn(LogbookModel::COLUMN_RST_SENT);
        ui->contactTable->showColumn(LogbookModel::COLUMN_FREQUENCY);
        ui->contactTable->showColumn(LogbookModel::COLUMN_MODE);
        ui->contactTable->showColumn(LogbookModel::COLUMN_NAME_INTL);
        ui->contactTable->showColumn(LogbookModel::COLUMN_QTH_INTL);
        ui->contactTable->showColumn(LogbookModel::COLUMN_COMMENT_INTL);
    }

    ui->contactTable->horizontalHeader()->setSectionsMovable(true);
    ui->contactTable->setStyle(new ProxyStyle(ui->contactTable->style()));

    ui->bandFilter->blockSignals(true);
    ui->bandFilter->setModel(new SqlListModel("SELECT name FROM bands ORDER BY start_freq", tr("Band"), this));
    ui->bandFilter->blockSignals(false);

    ui->modeFilter->blockSignals(true);
    ui->modeFilter->setModel(new SqlListModel("SELECT name FROM modes", tr("Mode"), this));
    ui->modeFilter->blockSignals(false);

    ui->countryFilter->blockSignals(true);
    countryModel = new SqlListModel("SELECT id, name FROM dxcc_entities WHERE id IN (SELECT DISTINCT dxcc FROM contacts) ORDER BY name;", tr("Country"), this);
    while (countryModel->canFetchMore())
    {
        countryModel->fetchMore();
    }
    ui->countryFilter->setModel(countryModel);
    ui->countryFilter->setModelColumn(1);
    ui->countryFilter->blockSignals(false);

    refreshClubFilter();

    ui->userFilter->blockSignals(true);
    userFilterModel = new SqlListModel("SELECT filter_name FROM qso_filters ORDER BY filter_name", tr("User Filter"), this);
    while (userFilterModel->canFetchMore())
    {
        userFilterModel->fetchMore();
    }
    ui->userFilter->setModel(userFilterModel);
    ui->userFilter->blockSignals(false);

    clublog = new ClubLog(this);

    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this, SLOT(focusSearchCallsign()), nullptr, Qt::ApplicationShortcut);

    restoreFilters();
}

void LogbookWidget::filterSelectedCallsign()
{
    FCT_IDENTIFICATION;

    QModelIndexList modeList= ui->contactTable->selectionModel()->selectedRows();
    if ( modeList.count() > 0 )
    {
        QSqlRecord record = model->record(modeList.first().row());
        filterCallsign(record.value("callsign").toString());
    }
}

void LogbookWidget::filterCountryBand(const QString &countryName, const QString &band,
                                      const QString &addlFilter)
{
    FCT_IDENTIFICATION;

    ui->countryFilter->blockSignals(true);
    ui->bandFilter->blockSignals(true);
    ui->userFilter->blockSignals(true);
    ui->modeFilter->blockSignals(true);
    ui->clubFilter->blockSignals(true);

    if ( ! countryName.isEmpty() )
    {
        ui->countryFilter->setCurrentText(countryName);
    }
    else
    {
        ui->countryFilter->setCurrentIndex(0);
    }

    if ( !band.isEmpty() )
    {
        ui->bandFilter->setCurrentText(band);
    }
    else
    {
        ui->bandFilter->setCurrentIndex(0);
    }

    //user wants to see only selected band and country
    ui->userFilter->setCurrentIndex(0); //suppress user-defined filter
    ui->modeFilter->setCurrentIndex(0); //suppress mode filter
    ui->clubFilter->setCurrentIndex(0); //suppress club filter

    // set additional filter
    externalFilter = addlFilter;

    ui->clubFilter->blockSignals(false);
    ui->userFilter->blockSignals(false);
    ui->modeFilter->blockSignals(false);
    ui->countryFilter->blockSignals(false);
    ui->bandFilter->blockSignals(false);

    updateTable();
}

void LogbookWidget::lookupSelectedCallsign() {
    FCT_IDENTIFICATION;

    QModelIndexList modeList = ui->contactTable->selectionModel()->selectedRows();
    if ( modeList.count() > 0)
    {
        QSqlRecord record = model->record(modeList.first().row());
        QString callsign = record.value("callsign").toString();
        QDesktopServices::openUrl(GenericCallbook::getWebLookupURL(callsign));
    }
}

void LogbookWidget::filterCallsign(const QString &call)
{
    FCT_IDENTIFICATION;

    if ( !call.isEmpty() )
    {
       ui->callsignFilter->setText(call);
    }
    else
    {
       ui->callsignFilter->setText("");
    }
}

void LogbookWidget::callsignFilterChanged()
{
    FCT_IDENTIFICATION;

    updateTable();
}

void LogbookWidget::bandFilterChanged()
{
    FCT_IDENTIFICATION;

    saveBandFilter();
    updateTable();
}

void LogbookWidget::saveBandFilter()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    settings.setValue("logbook/filters/band", ui->bandFilter->currentText());
}

void LogbookWidget::restoreBandFilter()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    ui->bandFilter->blockSignals(true);
    QString value = settings.value("logbook/filters/band").toString();
    if ( !value.isEmpty() )
    {
        ui->bandFilter->setCurrentText(value);
    }
    else
    {
        ui->bandFilter->setCurrentIndex(0);
    }
    ui->bandFilter->blockSignals(false);
}

void LogbookWidget::modeFilterChanged()
{
    FCT_IDENTIFICATION;

    saveModeFilter();
    updateTable();
}

void LogbookWidget::saveModeFilter()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    settings.setValue("logbook/filters/mode", ui->modeFilter->currentText());
}

void LogbookWidget::restoreModeFilter()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    ui->modeFilter->blockSignals(true);
    QString value = settings.value("logbook/filters/mode").toString();
    if ( !value.isEmpty() )
    {
        ui->modeFilter->setCurrentText(value);
    }
    else
    {
        ui->modeFilter->setCurrentIndex(0);
    }
    ui->modeFilter->blockSignals(false);
}

void LogbookWidget::countryFilterChanged()
{
    FCT_IDENTIFICATION;

    saveCountryFilter();
    updateTable();
}

void LogbookWidget::saveCountryFilter()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    settings.setValue("logbook/filters/country", ui->countryFilter->currentText());
}

void LogbookWidget::restoreCountryFilter()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    ui->countryFilter->blockSignals(true);
    QString value = settings.value("logbook/filters/country").toString();
    if ( !value.isEmpty() )
    {
        ui->countryFilter->setCurrentText(value);
    }
    else
    {
        ui->countryFilter->setCurrentIndex(0);
    }
    ui->countryFilter->blockSignals(false);
}

void LogbookWidget::userFilterChanged()
{
    FCT_IDENTIFICATION;

    saveUserFilter();
    updateTable();
}

void LogbookWidget::saveUserFilter()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    settings.setValue("logbook/filters/user", ui->userFilter->currentText());
}

void LogbookWidget::restoreUserFilter()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    ui->userFilter->blockSignals(true);
    QString value = settings.value("logbook/filters/user").toString();
    if ( !value.isEmpty() )
    {
        ui->userFilter->setCurrentText(value);
    }
    else
    {
        ui->userFilter->setCurrentIndex(0);
    }
    ui->userFilter->blockSignals(false);
}

void LogbookWidget::clubFilterChanged()
{
    FCT_IDENTIFICATION;

    saveClubFilter();
    updateTable();
}

void LogbookWidget::refreshClubFilter()
{
    FCT_IDENTIFICATION;

    ui->clubFilter->blockSignals(true);
    QString member = ui->clubFilter->currentText();
    ui->clubFilter->clear();
    ui->clubFilter->addItems(QStringList(tr("Club")) << MembershipQE::instance()->getEnabledClubLists());
    ui->clubFilter->setCurrentText(member);
    ui->clubFilter->blockSignals(false);
}

void LogbookWidget::saveClubFilter()
{
    QSettings settings;
    settings.setValue("logbook/filters/member", ui->clubFilter->currentText());
}

void LogbookWidget::restoreclubFilter()
{
    QSettings settings;
    ui->clubFilter->blockSignals(true);
    QString value = settings.value("logbook/filters/member").toString();
    if ( !value.isEmpty() )
    {
        ui->clubFilter->setCurrentText(value);
    }
    else
    {
        ui->clubFilter->setCurrentIndex(0);
    }
    ui->clubFilter->blockSignals(false);
}

void LogbookWidget::restoreFilters()
{
    FCT_IDENTIFICATION;

    restoreModeFilter();
    restoreBandFilter();
    restoreCountryFilter();
    restoreclubFilter();
    restoreUserFilter();
    externalFilter = QString();
    updateTable();
}

void LogbookWidget::uploadClublog() {
    FCT_IDENTIFICATION;

    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);

    AdiFormat adi(stream);

    foreach (QModelIndex index, ui->contactTable->selectionModel()->selectedRows()) {
        QSqlRecord record = model->record(index.row());
        adi.exportContact(record);
    }

    stream.flush();

    clublog->uploadAdif(data);
}

void LogbookWidget::deleteContact() {
    FCT_IDENTIFICATION;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Delete"), tr("Delete the selected contacts?"),
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    foreach (QModelIndex index, ui->contactTable->selectionModel()->selectedRows()) {
        model->removeRow(index.row());
    }
    ui->contactTable->clearSelection();
    updateTable();
}

void LogbookWidget::exportContact()
{
    FCT_IDENTIFICATION;

    QList<QSqlRecord>QSOs;
    auto selectedIndexes = ui->contactTable->selectionModel()->selectedRows();

    if ( selectedIndexes.count() < 1 )
    {
        return;
    }

    for (auto &index : qAsConst(selectedIndexes))
    {
        QSOs << model->record(index.row());
    }
    ExportDialog dialog(QSOs);
    dialog.exec();
}

void LogbookWidget::editContact()
{
    FCT_IDENTIFICATION;

    if ( ui->contactTable->selectionModel()->selectedRows().size() > 1 )
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      tr("Update"),
                                      tr("All selected rows will be updated to the same value.<br>Do you want to edit them?"),
                                      QMessageBox::Yes|QMessageBox::No);

        if (reply != QMessageBox::Yes) return;
    }

    ui->contactTable->edit(ui->contactTable->selectionModel()->currentIndex());
}

void LogbookWidget::displayedColumns()
{
    FCT_IDENTIFICATION;

    ColumnSettingDialog dialog(ui->contactTable);

    dialog.exec();

    saveTableHeaderState();
}

void LogbookWidget::updateTable()
{
    FCT_IDENTIFICATION;

    QStringList filterString;

    QString callsignFilterValue = ui->callsignFilter->text();

    if ( !callsignFilterValue.isEmpty() )
    {
        filterString.append(QString("callsign LIKE '%%1%'").arg(callsignFilterValue.toUpper()));
    }

    QString bandFilterValue = ui->bandFilter->currentText();

    if ( ui->bandFilter->currentIndex() != 0 && !bandFilterValue.isEmpty())
    {
        filterString.append(QString("band = '%1'").arg(bandFilterValue));
    }

    QString modeFilterValue = ui->modeFilter->currentText();

    if ( ui->modeFilter->currentIndex() != 0 && !modeFilterValue.isEmpty() )
    {
        filterString.append(QString("mode = '%1'").arg(modeFilterValue));
    }

    /* Refresh dynamic Country selection combobox */
    /* It is important to block its signals */
    ui->countryFilter->blockSignals(true);
    QString country = ui->countryFilter->currentText();
    countryModel->refresh();
    ui->countryFilter->setCurrentText(country);
    ui->countryFilter->blockSignals(false);

    int row = ui->countryFilter->currentIndex();
    QModelIndex idx = ui->countryFilter->model()->index(row,0);
    QVariant data = ui->countryFilter->model()->data(idx);

    if ( ui->countryFilter->currentIndex() != 0 )
    {
        filterString.append(QString("dxcc = '%1'").arg(data.toInt()));
    }

    if ( ui->clubFilter->currentIndex() != 0 )
    {
        filterString.append(QString("id in (SELECT contactid FROM contact_clubs_view WHERE clubid = '%1')").arg(ui->clubFilter->currentText()));
    }

    /* Refresh dynamic User Filter selection combobox */
    /* block the signals !!! */
    ui->userFilter->blockSignals(true);
    QString userFilterString = ui->userFilter->currentText();
    userFilterModel->refresh();
    ui->userFilter->setCurrentText(userFilterString);
    ui->userFilter->blockSignals(false);

    if ( ui->userFilter->currentIndex() != 0 )
    {
        QSqlQuery userFilterQuery;
        if ( ! userFilterQuery.prepare("SELECT "
                                "'(' || GROUP_CONCAT( ' ' || c.name || ' ' || CASE WHEN r.value IS NULL AND o.sql_operator IN ('=', 'like') THEN 'IS' "
                                "                                                  WHEN r.value IS NULL and r.operator_id NOT IN ('=', 'like') THEN 'IS NOT' "
                                "                                                  WHEN o.sql_operator = ('starts with') THEN 'like' "
                                "                                                  ELSE o.sql_operator END || "
                                "' (' || quote(CASE o.sql_operator WHEN 'like' THEN '%' || r.value || '%' "
                                "                                  WHEN 'not like' THEN '%' || r.value || '%' "
                                "                                  WHEN 'starts with' THEN r.value || '%' "
                                "                                  ELSE r.value END)  || ') ', m.sql_operator) || ')' "
                                "FROM qso_filters f, qso_filter_rules r, "
                                "qso_filter_operators o, qso_filter_matching_types m, "
                                "PRAGMA_TABLE_INFO('contacts') c "
                                "WHERE f.filter_name = :filterName "
                                "      AND f.filter_name = r.filter_name "
                                "      AND o.operator_id = r.operator_id "
                                "      AND m.matching_id = f.matching_type "
                                "      AND c.cid = r.table_field_index") )
        {
            qWarning() << "Cannot prepare select statement";
            return;
        }

        userFilterQuery.bindValue(":filterName", ui->userFilter->currentText());

        qCDebug(runtime) << "User filter SQL: " << userFilterQuery.lastQuery();

        if ( userFilterQuery.exec() )
        {
            userFilterQuery.next();
            filterString.append(QString("( ") + userFilterQuery.value(0).toString() + ")");
        }
        else
        {
            qCDebug(runtime) << "User filter error - " << userFilterQuery.lastError().text();
        }
    }

    if ( !externalFilter.isEmpty() )
    {
        filterString.append(QString("( ") + externalFilter + ")");
    }

    qCDebug(runtime) << "SQL filter summary: " << filterString.join(" AND ");
    model->setFilter(filterString.join(" AND "));
    qCDebug(runtime) << model->query().lastQuery();
    model->select();

    ui->contactTable->resizeColumnsToContents();

    emit logbookUpdated();
}

void LogbookWidget::saveTableHeaderState() {
    FCT_IDENTIFICATION;

    QSettings settings;
    QByteArray logbookState = ui->contactTable->horizontalHeader()->saveState();
    settings.setValue("logbook/state", logbookState);
}

void LogbookWidget::showTableHeaderContextMenu(const QPoint& point) {
    FCT_IDENTIFICATION;

    QMenu* contextMenu = new QMenu(this);
    for (int i = 0; i < model->columnCount(); i++) {
        QString name = model->headerData(i, Qt::Horizontal).toString();
        QAction* action = new QAction(name, contextMenu);
        action->setCheckable(true);
        action->setChecked(!ui->contactTable->isColumnHidden(i));

        connect(action, &QAction::triggered, this, [this, i]() {
            ui->contactTable->setColumnHidden(i, !ui->contactTable->isColumnHidden(i));
            saveTableHeaderState();
        });

        contextMenu->addAction(action);
    }

    contextMenu->exec(point);
}

void LogbookWidget::doubleClickColumn(QModelIndex modelIndex)
{
    FCT_IDENTIFICATION;


    /***********************/
    /* show EQSL QSL Image */
    /***********************/
    if ( modelIndex.column() == LogbookModel::COLUMN_EQSL_QSL_RCVD
         && modelIndex.data().toString() == 'Y')
    {
        QProgressDialog* dialog = new QProgressDialog(tr("Downloading eQSL Image"), tr("Cancel"), 0, 0, this);
        dialog->setWindowModality(Qt::WindowModal);
        dialog->setRange(0, 0);
        dialog->setAutoClose(true);
        dialog->show();

        EQSL *eQSL = new EQSL(dialog);

        connect(eQSL, &EQSL::QSLImageFound, this, [dialog, eQSL](QString imgFile)
        {
            dialog->done(0);
            QDesktopServices::openUrl(QUrl::fromLocalFile(imgFile));
            eQSL->deleteLater();
        });

        connect(eQSL, &EQSL::QSLImageError, this, [this, dialog, eQSL](const QString &error)
        {
            dialog->done(1);
            QMessageBox::critical(this, tr("QLog Error"), tr("eQSL Download Image failed: ") + error);
            eQSL->deleteLater();
        });

        connect(dialog, &QProgressDialog::canceled, this, [eQSL]()
        {
            qCDebug(runtime)<< "Operation canceled";
            eQSL->abortRequest();
            eQSL->deleteLater();
        });

        eQSL->getQSLImage(model->record(modelIndex.row()));
    }
    /**************************/
    /* show Paper QSL Manager */
    /**************************/
    else if ( modelIndex.column() == LogbookModel::COLUMN_QSL_RCVD
         && modelIndex.data().toString() == 'Y' )
    {
        PaperQSLDialog dialog(model->record(modelIndex.row()));
        dialog.exec();
    }
    /**************************************/
    /* show generic QSO Show/Edit Dialog  */
    /**************************************/
    else
    {
        QSODetailDialog dialog(model->record(modelIndex.row()));
        connect(&dialog, &QSODetailDialog::contactUpdated, this, [this](QSqlRecord& record)
        {
            emit contactUpdated(record);
        });
        dialog.exec();
        updateTable();
    }
}

void LogbookWidget::handleBeforeUpdate(int row, QSqlRecord &record)
{
    FCT_IDENTIFICATION;

    Q_UNUSED(row);
    emit contactUpdated(record);
}

void LogbookWidget::handleBeforeDelete(int row)
{
    FCT_IDENTIFICATION;

    QSqlRecord oldRecord = model->record(row);
    emit contactDeleted(oldRecord);
}

void LogbookWidget::focusSearchCallsign()
{
    FCT_IDENTIFICATION;

    ui->callsignFilter->setFocus();
}

void LogbookWidget::reloadSetting()
{
    FCT_IDENTIFICATION;
    /* Refresh dynamic Club selection combobox */
    refreshClubFilter();
}

LogbookWidget::~LogbookWidget() {
    FCT_IDENTIFICATION;

    saveTableHeaderState();
    delete ui;
}
