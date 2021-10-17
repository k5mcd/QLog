#include <QDebug>
#include <QSortFilterProxyModel>
#include <QScrollBar>

#include "WsjtxWidget.h"
#include "ui_WsjtxWidget.h"
#include "data/Data.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.wsjtxswidget");


bool operator==(const WsjtxEntry& a, const WsjtxEntry& b)
{
    return a.callsign == b.callsign;
}

int WsjtxTableModel::rowCount(const QModelIndex&) const
{
    return wsjtxData.count();
}

int WsjtxTableModel::columnCount(const QModelIndex&) const
{
    return 5;
}

QVariant WsjtxTableModel::data(const QModelIndex& index, int role) const
{
    WsjtxEntry entry = wsjtxData.at(index.row());

    if (role == Qt::DisplayRole)
    {
        switch ( index.column() )
        {
        case 0: return entry.callsign;
        case 1: return entry.grid;
        case 2: return QString::number(entry.decode.snr);
        case 3: return entry.decode.time.toString();
        case 4: return entry.decode.message;
        default: return QVariant();
        }
    }
    else if (index.column() == 0 && role == Qt::BackgroundRole)
    {
        return Data::statusToColor(entry.status, QColor(Qt::white));
    }
    else if (index.column() == 0 && role == Qt::TextColorRole)
    {
        return Data::statusToInverseColor(entry.status, QColor(Qt::black));
    }
    else if (index.column() == 0 && role == Qt::ToolTipRole)
    {
        return  entry.dxcc.country + " [" + Data::statusToText(entry.status) + "]";
    }
    return QVariant();
}

QVariant WsjtxTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    switch (section)
    {
    case 0: return tr("Callsign");
    case 1: return tr("Grid");
    case 2: return tr("SNR");
    case 3: return tr("Last Activity");
    case 4: return tr("Last Message");
    default: return QVariant();
    }
}

void WsjtxTableModel::addOrReplaceEntry(WsjtxEntry entry)
{
    FCT_IDENTIFICATION;


    int idx = wsjtxData.indexOf(entry);

    if ( idx >= 0 )
    {
        qCDebug(runtime) << "Updating " << entry.callsign;

        if ( ! entry.grid.isEmpty() )
        {
            wsjtxData[idx].grid = entry.grid;
        }

        wsjtxData[idx].status = entry.status;
        wsjtxData[idx].decode = entry.decode;
        wsjtxData[idx].receivedTime = entry.receivedTime;

        emit dataChanged(createIndex(idx,0), createIndex(idx,4));
    }
    else
    {
       qCDebug(runtime) << "Inserting " << entry.callsign;
       beginInsertRows(QModelIndex(), wsjtxData.count(), wsjtxData.count());
       wsjtxData.append(entry);
       endInsertRows();
    }
}

void WsjtxTableModel::spotAging()
{
    FCT_IDENTIFICATION;

    beginResetModel();

    for (auto entry: wsjtxData )
    {
        if ( entry.receivedTime.secsTo(QDateTime::currentDateTimeUtc()) > spotAgingPeriod )
        {
            qCDebug(runtime) << "Removing " << entry.callsign;
            wsjtxData.removeOne(entry);
        }
    }
    endResetModel();
}

bool WsjtxTableModel::callsignExists(WsjtxEntry call)
{
    FCT_IDENTIFICATION;

    return wsjtxData.contains(call);
}

QString WsjtxTableModel::getCallsign(QModelIndex idx)
{
    FCT_IDENTIFICATION;

    return data(index(idx.row(),0),Qt::DisplayRole).toString();
}

QString WsjtxTableModel::getGrid(QModelIndex idx)
{
    FCT_IDENTIFICATION;

    return data(index(idx.row(),1),Qt::DisplayRole).toString();
}

WsjtxDecode WsjtxTableModel::getDecode(QModelIndex idx)
{
    FCT_IDENTIFICATION;
    return wsjtxData.at(idx.row()).decode;
}

void WsjtxTableModel::setSpotAging(int seconds)
{
    FCT_IDENTIFICATION;

    spotAgingPeriod = seconds;
}

WsjtxWidget::WsjtxWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WsjtxWidget)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    wsjtxTableModel = new WsjtxTableModel(this);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(wsjtxTableModel);

    ui->tableView->setModel(proxyModel);
}

void WsjtxWidget::decodeReceived(WsjtxDecode decode)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<decode.message;

    if ( decode.message.startsWith("CQ") )
    {
        QRegExp cqRegExp("^CQ (DX |TEST |[A-Z]{0,2} )?([A-Z0-9\/]+) ?([A-Z]{2}[0-9]{2})?.*");
        if ( cqRegExp.exactMatch(decode.message) )
        {
            WsjtxEntry entry;

            entry.decode = decode;
            entry.callsign = cqRegExp.cap(2);
            entry.grid = cqRegExp.cap(3);
            entry.dxcc = Data::instance()->lookupDxcc(entry.callsign);
            entry.status = Data::instance()->dxccStatus(entry.dxcc.dxcc, band, status.mode);
            entry.receivedTime = QDateTime::currentDateTimeUtc();

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

                wsjtxTableModel->addOrReplaceEntry(entry);
            }
        }
    }

    wsjtxTableModel->spotAging();
    proxyModel->sort(3, Qt::DescendingOrder);

    ui->tableView->repaint();

}

void WsjtxWidget::statusReceived(WsjtxStatus newStatus)
{
    FCT_IDENTIFICATION;

    if (this->status.dial_freq != newStatus.dial_freq) {
        band = Data::instance()->band(newStatus.dial_freq/1e6).name;
        ui->freqLabel->setText(QString("%1 MHz").arg(newStatus.dial_freq/1e6));
    }

    if (status.transmitting) {
        ui->txStatus->setText(tr("Transmitting"));
    }
    else {
        ui->txStatus->setText(tr("Monitoring"));
    }

    status = newStatus;

    ui->modeLabel->setText(status.mode);

    if ( status.mode == "FT8" )
    {
        wsjtxTableModel->setSpotAging(55);
    }
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

WsjtxWidget::~WsjtxWidget()
{
    FCT_IDENTIFICATION;

    delete ui;
}
