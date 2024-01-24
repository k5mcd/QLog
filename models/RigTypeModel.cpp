#include "RigTypeModel.h"
#include "rig/Rig.h"

RigTypeModel::RigTypeModel(QObject* parent)
    : QAbstractListModel(parent)
{

}

int RigTypeModel::rowCount(const QModelIndex&) const {
    return rigList.count();
}

QVariant RigTypeModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole) {
        return rigList.value(index.row());
    }

    if (role == Qt::UserRole )
    {
        return rigIds[rigList.value(index.row())];
    }
    return QVariant();
}

QModelIndex RigTypeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    int rigId = rigIds[rigList.value(row)];
    if (rigId)
        return createIndex(row, column, rigId);
    else
        return QModelIndex();
}

void RigTypeModel::select(int driverID)
{
    beginResetModel();
    rigIds.clear();
    rigList.clear();

    if ( driverID == 0 )
        return;

    const QList<QPair<int, QString>> models = Rig::instance()->getModelList(static_cast<Rig::DriverID>(driverID));
    for ( const QPair<int, QString> &model : models )
    {
        const QString &name = model.second;
        rigIds[name] = model.first;
        rigList.append(name);
    }
    rigList.sort();
    endResetModel();
}
