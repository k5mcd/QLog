#include "AlertTableModel.h"
#include "core/debug.h"


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

    SpotAlert selectedAlert = alertList.at(index.row());

    if (role == Qt::DisplayRole)
    {
        switch ( index.column() )
        {
        case 0: return selectedAlert.dateTime.toString(locale.timeFormat(QLocale::LongFormat)).remove("UTC");;
        case 1: return selectedAlert.ruleName.join(", ");
        case 2: return selectedAlert.callsign;
        case 3: return QString::number(selectedAlert.freq, 'f', 5);
        case 4: return selectedAlert.mode;
        case 5: return selectedAlert.comment;
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
    case 0: return tr("Time");
    case 1: return tr("Rules");
    case 2: return tr("Callsign");
    case 3: return tr("Frequency");
    case 4: return tr("Mode");
    case 5: return tr("Comment");
    default: return QVariant();
    }
}

void AlertTableModel::addAlert(SpotAlert entry)
{
    alertListMutex.lock();
    beginInsertRows(QModelIndex(), alertList.count(), alertList.count());
    alertList.prepend(entry);
    endInsertRows();
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

