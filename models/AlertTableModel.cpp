#include "AlertTableModel.h"
#include "core/debug.h"

//-+ FREQ_MATCH_TOLERANCE MHz is OK when QLog evaluates the same spot freq
#define FREQ_MATCH_TOLERANCE 0.005

int AlertTableModel::rowCount(const QModelIndex&) const
{
    return alertList.count();
}

int AlertTableModel::columnCount(const QModelIndex&) const
{
    return 6;
}

QVariant AlertTableModel::data(const QModelIndex& index, int role) const
{
    QLocale locale;

    AlertTableRecord selectedRecord = alertList.at(index.row());

    if (role == Qt::DisplayRole)
    {
        switch ( index.column() )
        {
        case 0: return selectedRecord.callsign;
        case 1: return QString::number(selectedRecord.freq, 'f', 5);
        case 2: return selectedRecord.mode;
        case 3: return selectedRecord.counter;
        case 4: return selectedRecord.dateTime.toString(locale.timeFormat(QLocale::LongFormat)).remove("UTC");
        case 5: return selectedRecord.comment;
        default: return QVariant();
        }
    }

    return QVariant();
}

QVariant AlertTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    switch (section)
    {
    case 0: return tr("Callsign");
    case 1: return tr("Frequency");
    case 2: return tr("Mode");
    case 3: return tr("Updated");
    case 4: return tr("Last Update");
    case 5: return tr("Last Comment");
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
        emit dataChanged(createIndex(spotIndex,0), createIndex(spotIndex,5));
    }
    else
    {
        /* New spot, insert it */
        beginInsertRows(QModelIndex(), alertList.count(), alertList.count());
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


bool AlertTableModel::AlertTableRecord::operator==(const AlertTableRecord &spot)
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
    mode(spotAlert.mode),
    comment(spotAlert.comment),
    counter(0)
{

}
