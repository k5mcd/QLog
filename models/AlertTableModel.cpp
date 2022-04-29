#include "AlertTableModel.h"
#include "core/debug.h"


int AlertTableModel::rowCount(const QModelIndex&) const
{
    return alertList.count();
}

int AlertTableModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant AlertTableModel::data(const QModelIndex& index, int role) const
{
    UserAlert selectedAlert = alertList.at(index.row());

    if (role == Qt::DisplayRole)
    {
        switch ( index.column() )
        {
        case 0: return selectedAlert.test;
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
    case 0: return tr("Alert Text");
    default: return QVariant();
    }
}

void AlertTableModel::addAlert(UserAlert entry)
{
    beginInsertRows(QModelIndex(), alertList.count(), alertList.count());
    alertList.append(entry);
    endInsertRows();

}

void AlertTableModel::clear()
{
    beginResetModel();
    alertList.clear();
    endResetModel();
}

