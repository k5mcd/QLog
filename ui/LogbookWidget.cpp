#include <QtSql>
#include <QMessageBox>
#include <QDesktopServices>
#include <QMenu>
#include "logformat/AdiFormat.h"
#include "models/LogbookModel.h"
#include "models/SqlListModel.h"
#include "core/ClubLog.h"
#include "LogbookWidget.h"
#include "ui_LogbookWidget.h"
#include "core/StyleItemDelegate.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.logbookwidget");

LogbookWidget::LogbookWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogbookWidget)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    model = new LogbookModel(this);
    ui->contactTable->setModel(model);

    ui->contactTable->addAction(ui->actionFilter);
    ui->contactTable->addAction(ui->actionLookup);
    ui->contactTable->addAction(ui->actionUploadClublog);
    ui->contactTable->addAction(ui->actionDeleteContact);
    //ui->contactTable->sortByColumn(1, Qt::DescendingOrder);

    ui->contactTable->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->contactTable->horizontalHeader(), &QHeaderView::customContextMenuRequested,
            this, &LogbookWidget::showTableHeaderContextMenu);

    ui->contactTable->setItemDelegateForColumn(1, new TimestampFormatDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(2, new TimestampFormatDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(3, new CallsignDelegate(ui->contactTable));
    ui->contactTable->setItemDelegateForColumn(6, new UnitFormatDelegate("MHz", 6, 0.001, ui->contactTable));

    QSettings settings;
    QVariant logbookState = settings.value("logbook/state");
    if (!logbookState.isNull()) {
        ui->contactTable->horizontalHeader()->restoreState(logbookState.toByteArray());
    }
    else {
        ui->contactTable->hideColumn(0);
        ui->contactTable->hideColumn(2);
        ui->contactTable->hideColumn(9);
        ui->contactTable->hideColumn(13);
        ui->contactTable->hideColumn(15);
        ui->contactTable->hideColumn(18);
        ui->contactTable->hideColumn(19);
        ui->contactTable->hideColumn(24);
        ui->contactTable->hideColumn(26);
        ui->contactTable->hideColumn(28);
        ui->contactTable->hideColumn(30);
    }

    ui->contactTable->horizontalHeader()->setSectionsMovable(true);

    ui->bandFilter->setModel(new SqlListModel("SELECT name FROM bands ORDER BY start_freq", "Band"));
    ui->modeFilter->setModel(new SqlListModel("SELECT name FROM modes", "Mode"));
    ui->countryFilter->setModel(new SqlListModel("SELECT name FROM dxcc_entities ORDER BY name", "Country"));

    clublog = new ClubLog(this);

    updateTable();
}

void LogbookWidget::filterSelectedCallsign() {
    FCT_IDENTIFICATION;

    QModelIndexList modeList= ui->contactTable->selectionModel()->selectedRows();
    if ( modeList.count() > 0 )
    {
        QSqlRecord record = model->record(modeList.first().row());
        filterCallsign(record.value("callsign").toString());
    }
}

void LogbookWidget::lookupSelectedCallsign() {
    FCT_IDENTIFICATION;

    QModelIndexList modeList = ui->contactTable->selectionModel()->selectedRows();
    if ( modeList.count() > 0)
    {

        QSqlRecord record = model->record(modeList.first().row());
        QString callsign = record.value("callsign").toString();
        QDesktopServices::openUrl(QString("https://www.qrz.com/lookup/%1").arg(callsign));
    }

}

void LogbookWidget::filterCallsign(QString call) {
    FCT_IDENTIFICATION;

    if ( !call.isEmpty() )
    {
       ui->callsignFilter->setText(call);
    }
}

void LogbookWidget::callsignFilterChanged() {
    FCT_IDENTIFICATION;

    QString callsign = ui->callsignFilter->text();
    if (!callsign.isEmpty()) {
        model->setFilter(QString("callsign LIKE '%1%'").arg(ui->callsignFilter->text()));
    }
    else {
        model->setFilter(nullptr);
    }
    updateTable();
}

void LogbookWidget::bandFilterChanged() {
    FCT_IDENTIFICATION;

    QString band = ui->bandFilter->currentText();
    if (ui->bandFilter->currentIndex() != 0 && !band.isEmpty()) {
        model->setFilter(QString("band = '%1'").arg(band));
    }
    else {
        model->setFilter(nullptr);
    }
    updateTable();
}

void LogbookWidget::modeFilterChanged() {
    FCT_IDENTIFICATION;

    QString mode = ui->modeFilter->currentText();
    if (ui->modeFilter->currentIndex() != 0 && !mode.isEmpty()) {
        model->setFilter(QString("mode = '%1'").arg(mode));
    }
    else {
        model->setFilter(nullptr);
    }
    updateTable();
}

void LogbookWidget::countryFilterChanged() {
    FCT_IDENTIFICATION;

    QString country = ui->countryFilter->currentText();
    if (ui->countryFilter->currentIndex() != 0 && !country.isEmpty()) {
        model->setFilter(QString("country = '%1'").arg(country));
    }
    else {
        model->setFilter(nullptr);
    }
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

void LogbookWidget::updateTable() {
    FCT_IDENTIFICATION;

    model->select();
    ui->contactTable->resizeColumnsToContents();
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

        connect(action, &QAction::triggered, [this, i]() {
            ui->contactTable->setColumnHidden(i, !ui->contactTable->isColumnHidden(i));
            saveTableHeaderState();
        });

        contextMenu->addAction(action);
    }
    contextMenu->exec(point);
}

LogbookWidget::~LogbookWidget() {
    FCT_IDENTIFICATION;

    saveTableHeaderState();
    delete ui;
}
