#ifndef QLOG_MODELS_AWARDSTABLEMODEL_H
#define QLOG_MODELS_AWARDSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QSqlTableModel>

class AwardsTableModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit AwardsTableModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // QLOG_MODELS_AWARDSTABLEMODEL_H
