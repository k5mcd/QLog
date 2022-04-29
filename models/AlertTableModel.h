#ifndef ALERTTABLEMODEL_H
#define ALERTTABLEMODEL_H

#include <QAbstractTableModel>
#include <data/UserAlert.h>

class AlertTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    AlertTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent){};
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void addAlert(UserAlert entry);
    void clear();

private:
    QList<UserAlert> alertList;
};

#endif // ALERTTABLEMODEL_H
