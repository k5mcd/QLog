#include <QColor>
#include "AlertTableModel.h"
#include "core/debug.h"
#include "data/Data.h"

//-+ FREQ_MATCH_TOLERANCE MHz is OK when QLog evaluates the same spot freq
#define FREQ_MATCH_TOLERANCE 0.005

int AlertTableModel::rowCount(const QModelIndex&) const
{
    return alertList.count();
}

int AlertTableModel::columnCount(const QModelIndex&) const
{
    return 7;
}

QVariant AlertTableModel::data(const QModelIndex& index, int role) const
{
    AlertTableRecord selectedRecord = alertList.at(index.row());

    if (role == Qt::DisplayRole)
    {
        switch ( index.column() )
        {
        case 0: return selectedRecord.ruleName.join(",");
        case 1: return selectedRecord.callsign;
        case 2: return QString::number(selectedRecord.freq, 'f', 5);
        case 3: return selectedRecord.mode;
        case 4: return selectedRecord.counter;
        case 5: return selectedRecord.dateTime.toString(locale.formatTimeLongWithoutTZ());
        case 6: return selectedRecord.comment;
        default: return QVariant();
        }
    }
    else if ( index.column() == 1 && role == Qt::BackgroundRole )
    {
        return Data::statusToColor(selectedRecord.status, QColor(Qt::transparent));
    }

    return QVariant();
}

QVariant AlertTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    switch (section)
    {
    case 0: return tr("Rule Name");
    case 1: return tr("Callsign");
    case 2: return tr("Frequency");
    case 3: return tr("Mode");
    case 4: return tr("Updated");
    case 5: return tr("Last Update");
    case 6: return tr("Last Comment");
    default: return QVariant();
    }
}

void AlertTableModel::addAlert(SpotAlert entry)
{
    AlertTableRecord newRecord(entry);

    alertListMutex.lock();

    int spotIndex = alertList.indexOf(newRecord);

    if ( spotIndex >= 0)
    {
        /* QLog already contains the spot, update it */
        alertList[spotIndex].freq = newRecord.freq;
        alertList[spotIndex].counter++;
        alertList[spotIndex].dateTime = newRecord.dateTime;
        alertList[spotIndex].comment = newRecord.comment;
        alertList[spotIndex].ruleName << entry.ruleName;
        alertList[spotIndex].ruleName.removeDuplicates();
        alertList[spotIndex].ruleName.sort();
        emit dataChanged(createIndex(spotIndex,0), createIndex(spotIndex,5));
    }
    else
    {
        /* New spot, insert it */
        beginInsertRows(QModelIndex(), 0, 0);
        alertList.prepend(newRecord);
        endInsertRows();
    }
    alertListMutex.unlock();
}

void AlertTableModel::clear()
{
    alertListMutex.lock();
    beginResetModel();
    alertList.clear();
    endResetModel();
    alertListMutex.unlock();
}

QString AlertTableModel::getCallsign(const QModelIndex &index)
{
    alertListMutex.lock();
    QString ret = alertList.at(index.row()).callsign;
    alertListMutex.unlock();
    return ret;
}

double AlertTableModel::getFrequency(const QModelIndex &index)
{
    alertListMutex.lock();
    double ret = alertList.at(index.row()).freq;
    alertListMutex.unlock();
    return ret;
}

void AlertTableModel::aging(const int clear_interval_sec)
{
    if ( clear_interval_sec <= 0 ) return;

    alertListMutex.lock();

    QMutableListIterator<AlertTableRecord> alertIterator(alertList);

    beginResetModel();
    while ( alertIterator.hasNext() )
    {
        alertIterator.next();
        if ( alertIterator.value().dateTime.addSecs(clear_interval_sec) <= QDateTime::currentDateTimeUtc() )
        {

            alertIterator.remove();

        }
    }
    endResetModel();

    alertListMutex.unlock();
}


bool AlertTableModel::AlertTableRecord::operator==(const AlertTableRecord &spot) const
{
   return ( (spot.callsign == this->callsign)
            && (spot.mode == this->mode)
            && (qAbs(this->freq - spot.freq) <= FREQ_MATCH_TOLERANCE)
            );
}

AlertTableModel::AlertTableRecord::AlertTableRecord(const SpotAlert &spotAlert) :
    dateTime(spotAlert.dateTime),
    ruleName(spotAlert.ruleName),
    callsign(spotAlert.callsign),
    freq(spotAlert.freq),
    band(spotAlert.band),
    mode(spotAlert.modeGroupString),
    comment(spotAlert.comment),
    counter(0),
    status(spotAlert.status)
{

}
