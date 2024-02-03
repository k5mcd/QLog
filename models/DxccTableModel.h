#ifndef QLOG_MODELS_DXCCTABLEMODEL_H
#define QLOG_MODELS_DXCCTABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>

class DxccTableModel : public QSqlQueryModel
{
public:
    explicit DxccTableModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex &, int role = Qt::DisplayRole) const;
};

#endif // QLOG_MODELS_DXCCTABLEMODEL_H
