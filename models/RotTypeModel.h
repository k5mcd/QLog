#ifndef QLOG_MODELS_ROTTYPEMODEL_H
#define QLOG_MODELS_ROTTYPEMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class RotTypeModel : public QAbstractListModel {
    Q_OBJECT

public:
    RotTypeModel(QObject* parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    void select(int driverID);

private:
    QStringList rotList;
    QMap<QString, int> rotIds;
};

#endif // QLOG_MODELS_RIGTYPEMODEL_H
