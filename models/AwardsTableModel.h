#ifndef AWARDSTABLEMODEL_H
#define AWARDSTABLEMODEL_H

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

#endif // AWARDSTABLEMODEL_H
