#ifndef SQLLISTMODEL_H
#define SQLLISTMODEL_H

#include <QSqlQueryModel>

class SqlListModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit SqlListModel(const QString &, const QString &, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int, Qt::Orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &, int role = Qt::DisplayRole) const override;

    void refresh();
private:
    QString placeholder;
    QString stmt;
};

#endif // SQLLISTMODEL_H
