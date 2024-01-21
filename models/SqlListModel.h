#ifndef QLOG_MODELS_SQLLISTMODEL_H
#define QLOG_MODELS_SQLLISTMODEL_H

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

#endif // QLOG_MODELS_SQLLISTMODEL_H
