#include <QDebug>
#include <QSortFilterProxyModel>
#include <QScrollBar>
#include <QMutableListIterator>

#include "WsjtxWidget.h"
#include "ui_WsjtxWidget.h"
#include "data/Data.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.wsjtxswidget");

WsjtxWidget::WsjtxWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WsjtxWidget),
    lastSelectedCallsign(QString())
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    wsjtxTableModel = new WsjtxTableModel(this);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(wsjtxTableModel);

    ui->tableView->setModel(proxyModel);
    ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void WsjtxWidget::decodeReceived(WsjtxDecode decode)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<decode.message;

    if ( decode.message.startsWith("CQ") )
    {
        QRegExp cqRegExp("^CQ (DX |TEST |[A-Z]{0,2} )?([A-Z0-9\\/]+) ?([A-Z]{2}[0-9]{2})?.*");
        if ( cqRegExp.exactMatch(decode.message) )
        {
            WsjtxEntry entry;

            entry.decode = decode;
            entry.callsign = cqRegExp.cap(2);
            entry.grid = cqRegExp.cap(3);
            entry.dxcc = Data::instance()->lookupDxcc(entry.callsign);
            entry.status = Data::instance()->dxccStatus(entry.dxcc.dxcc, band, status.mode);
            entry.receivedTime = QDateTime::currentDateTimeUtc();
            entry.freq = currFreq;
            entry.band = band;
            entry.decodedMode = currMode;

            emit CQSpot(entry);
            wsjtxTableModel->addOrReplaceEntry(entry);
        }
    }
    else
    {
        QStringList decodedElements = decode.message.split(" ");

        if ( decodedElements.count() > 1 )
        {
            QString callsign = decode.message.split(" ").at(1);
            WsjtxEntry entry;
            entry.callsign = callsign;
            if ( wsjtxTableModel->callsignExists(entry) )
            {
                entry.dxcc = Data::instance()->lookupDxcc(entry.callsign);
                entry.status = Data::instance()->dxccStatus(entry.dxcc.dxcc, band, status.mode);
                entry.decode = decode;
                entry.receivedTime = QDateTime::currentDateTimeUtc();
                entry.freq = currFreq;
                entry.band = band;
                entry.decodedMode = currMode;

                wsjtxTableModel->addOrReplaceEntry(entry);
            }
        }
    }

    wsjtxTableModel->spotAging();
    proxyModel->sort(4, Qt::DescendingOrder);

    ui->tableView->repaint();

    setSelectedCallsign(lastSelectedCallsign);
}

void WsjtxWidget::statusReceived(WsjtxStatus newStatus)
{
    FCT_IDENTIFICATION;

    if (this->status.dial_freq != newStatus.dial_freq) {
        currFreq = newStatus.dial_freq/1e6;
        band = Data::instance()->band(currFreq).name;
        ui->freqLabel->setText(QString("%1 MHz").arg(currFreq));
        wsjtxTableModel->clear();
    }

    if ( this->status.dx_call != newStatus.dx_call )
    {
        lastSelectedCallsign = newStatus.dx_call;
        setSelectedCallsign(lastSelectedCallsign);
        emit showDxDetails(newStatus.dx_call, newStatus.dx_grid);
    }

    if ( this->status.mode != newStatus.mode )
    {
        ui->modeLabel->setText(newStatus.mode);
        currMode = newStatus.mode;
        wsjtxTableModel->setCurrentSpotPeriod(Wsjtx::modePeriodLenght(newStatus.mode)); /*currently, only Status has a correct Mode in the message */
    }

    status = newStatus;
    wsjtxTableModel->spotAging();
    ui->tableView->repaint();
}

void WsjtxWidget::tableViewDoubleClicked(QModelIndex index)
{
    FCT_IDENTIFICATION;

    QModelIndex source_index = proxyModel->mapToSource(index);
    QString callsign = wsjtxTableModel->getCallsign(source_index);
    QString grid = wsjtxTableModel->getGrid(source_index);
    emit showDxDetails(callsign, grid);
    emit reply(wsjtxTableModel->getDecode(source_index));
}

void WsjtxWidget::tableViewClicked(QModelIndex index)
{
    FCT_IDENTIFICATION;

    QModelIndex source_index = proxyModel->mapToSource(index);
    lastSelectedCallsign = wsjtxTableModel->getCallsign(source_index);
}

void WsjtxWidget::setSelectedCallsign(const QString &selectCallsign)
{
    FCT_IDENTIFICATION;

    QModelIndexList nextMatches = proxyModel->match(proxyModel->index(0,0), Qt::DisplayRole, selectCallsign, 1);

    if ( nextMatches.size() >= 1 )
    {
        ui->tableView->setCurrentIndex(nextMatches.at(0));
    }
}

WsjtxWidget::~WsjtxWidget()
{
    FCT_IDENTIFICATION;

    delete ui;
}
