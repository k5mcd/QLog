#ifndef LOGBOOKMODEL_H
#define LOGBOOKMODEL_H

#include <QObject>
#include <QSqlTableModel>

class LogbookModel : public QSqlTableModel
{
    Q_OBJECT

public:
    explicit LogbookModel(QObject* parent = nullptr, QSqlDatabase db = QSqlDatabase());

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    enum column_id
    {
        COLUMN_ID = 0,
        COLUMN_TIME_ON = 1,
        COLUMN_TIME_OFF = 2,
        COLUMN_CALL = 3,
        COLUMN_RST_SENT = 4,
        COLUMN_RST_RCVD = 5,
        COLUMN_FREQUENCY = 6,
        COLUMN_BAND = 7,
        COLUMN_MODE = 8,
        COLUMN_SUBMODE = 9,
        COLUMN_NAME = 10,
        COLUMN_QTH = 11,
        COLUMN_GRID = 12,
        COLUMN_DXCC = 13,
        COLUMN_COUNTRY = 14,
        COLUMN_CONTINENT = 15,
        COLUMN_CQZ = 16,
        COLUMN_ITUZ = 17,
        COLUMN_PREFIX = 18,
        COLUMN_STATE = 19,
        COLUMN_COUNTY = 20,
        COLUMN_IOTA = 21,
        COLUMN_QSL_RCVD = 22,
        COLUMN_QSL_RCVD_DATE = 23,
        COLUMN_QSL_SENT = 24,
        COLUMN_QSL_SENT_DATE = 25,
        COLUMN_LOTW_RCVD = 26,
        COLUMN_LOTW_RCVD_DATE = 27,
        COLUMN_LOTW_SENT = 28,
        COLUMN_LOTW_SENT_DATE = 29,
        COLUMN_TX_POWER = 30,
        COLUMN_FIELDS = 31
    };
};

#endif // LOGBOOKMODEL_H
