#ifndef QLOG_MODELS_RIGTYPEMODEL_H
#define QLOG_MODELS_RIGTYPEMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class RigTypeModel : public QAbstractListModel {
    Q_OBJECT

public:
    RigTypeModel(QObject* parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    void select(int driverID);

private:
    QStringList rigList;
    QMap<QString, int> rigIds;
};

#endif // QLOG_MODELS_RIGTYPEMODEL_H
