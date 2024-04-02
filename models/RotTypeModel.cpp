#include "rotator/Rotator.h"
#include "RotTypeModel.h"

RotTypeModel::RotTypeModel(QObject* parent)
    : QAbstractListModel(parent)
{

}

int RotTypeModel::rowCount(const QModelIndex&) const {
    return rotList.count();
}

QVariant RotTypeModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole) {
        return rotList.value(index.row());
    }

    if (role == Qt::UserRole )
    {
        return rotIds[rotList.value(index.row())];
    }
    return QVariant();
}

QModelIndex RotTypeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    int rotId = rotIds[rotList.value(row)];
    if (rotId)
        return createIndex(row, column, rotId);
    else
        return QModelIndex();
}

void RotTypeModel::select(int driverID)
{
    beginResetModel();
    rotIds.clear();
    rotList.clear();

    if ( driverID == 0 )
        return;

    const QList<QPair<int, QString>> models = Rotator::instance()->getModelList(static_cast<Rotator::DriverID>(driverID));
    for ( const QPair<int, QString> &model : models )
    {
        const QString &name = model.second;
        rotIds[name] = model.first;
        rotList.append(name);
    }
    rotList.sort();
    endResetModel();
}

