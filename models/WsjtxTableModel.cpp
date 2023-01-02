#include <QColor>
#include "WsjtxTableModel.h"
#include "core/debug.h"
#include "data/StationProfile.h"
#include "core/Gridsquare.h"

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
    return 6;
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
        case 2: if ( entry.distance > 0.0 ) return entry.distance; else QVariant();
        case 3: return QString::number(entry.decode.snr);
        case 4: return entry.decode.time.toString();
        case 5: return entry.decode.message;
        default: return QVariant();
        }
    }
    else if (index.column() == 0 && role == Qt::BackgroundRole)
    {
        return Data::statusToColor(entry.status, QColor(Qt::transparent));
    }
    else if (index.column() > 0 && role == Qt::BackgroundRole)
    {
        if ( entry.receivedTime.secsTo(QDateTime::currentDateTimeUtc()) >= spotPeriod * 0.8)
            /* -20% time of period because WSTX sends messages in waves and not exactly in time period */
        {
            return QColor(Qt::darkGray);
        }
    }
    else if (index.column() == 0 && role == Qt::ForegroundRole)
    {
        //return Data::statusToInverseColor(entry.status, QColor(Qt::black));
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
    case 2: return tr("Distance");
    case 3: return tr("SNR");
    case 4: return tr("Last Activity");
    case 5: return tr("Last Message");
    default: return QVariant();
    }
}

void WsjtxTableModel::addOrReplaceEntry(WsjtxEntry entry)
{
    int idx = wsjtxData.indexOf(entry);

    if ( idx >= 0 )
    {
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
        beginInsertRows(QModelIndex(), wsjtxData.count(), wsjtxData.count());
        wsjtxData.append(entry);
        endInsertRows();
    }
}

void WsjtxTableModel::spotAging()
{
    beginResetModel();

    QMutableListIterator<WsjtxEntry> entry(wsjtxData);

    while ( entry.hasNext() )
    {
        WsjtxEntry current = entry.next();

        if ( current.receivedTime.secsTo(QDateTime::currentDateTimeUtc()) > (3.0 * spotPeriod)*1.2 )
            /* +20% time of period because WSTX sends messages in waves and not exactly in time period */
        {
            entry.remove();
        }
    }

    endResetModel();
}

bool WsjtxTableModel::callsignExists(const WsjtxEntry &call)
{
    return wsjtxData.contains(call);
}

QString WsjtxTableModel::getCallsign(QModelIndex idx)
{
    return data(index(idx.row(),0),Qt::DisplayRole).toString();
}

QString WsjtxTableModel::getGrid(QModelIndex idx)
{
    return data(index(idx.row(),1),Qt::DisplayRole).toString();
}

WsjtxDecode WsjtxTableModel::getDecode(QModelIndex idx)
{
    return wsjtxData.at(idx.row()).decode;
}

void WsjtxTableModel::setCurrentSpotPeriod(float period)
{
    spotPeriod = period;
}

void WsjtxTableModel::clear()
{
    beginResetModel();
    wsjtxData.clear();
    endResetModel();
}

