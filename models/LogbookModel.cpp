#include "LogbookModel.h"
#include "data/Data.h"
#include "core/utils.h"
#include "data/Dxcc.h"

#include <QIcon>

LogbookModel::LogbookModel(QObject* parent, QSqlDatabase db)
        : QSqlTableModel(parent, db)
{
    setTable("contacts");
    setEditStrategy(QSqlTableModel::OnFieldChange);
    setSort(COLUMN_TIME_ON, Qt::DescendingOrder);

    setHeaderData(COLUMN_ID, Qt::Horizontal, tr("ID"));
    setHeaderData(COLUMN_TIME_ON, Qt::Horizontal, tr("Time on"));
    setHeaderData(COLUMN_TIME_OFF, Qt::Horizontal, tr("Time off"));
    setHeaderData(COLUMN_CALL, Qt::Horizontal, tr("Call"));
    setHeaderData(COLUMN_RST_SENT, Qt::Horizontal, tr("RST Sent"));
    setHeaderData(COLUMN_RST_RCVD, Qt::Horizontal, tr("RST Rcvd"));
    setHeaderData(COLUMN_FREQUENCY, Qt::Horizontal, tr("Frequency"));
    setHeaderData(COLUMN_BAND, Qt::Horizontal, tr("Band"));
    setHeaderData(COLUMN_MODE, Qt::Horizontal, tr("Mode"));
    setHeaderData(COLUMN_SUBMODE, Qt::Horizontal, tr("Submode"));
    setHeaderData(COLUMN_NAME, Qt::Horizontal, tr("Name"));
    setHeaderData(COLUMN_QTH, Qt::Horizontal, tr("QTH"));
    setHeaderData(COLUMN_GRID, Qt::Horizontal, tr("Gridsquare"));
    setHeaderData(COLUMN_DXCC, Qt::Horizontal, tr("DXCC"));
    setHeaderData(COLUMN_COUNTRY, Qt::Horizontal, tr("Country"));
    setHeaderData(COLUMN_CONTINENT, Qt::Horizontal, tr("Continent"));
    setHeaderData(COLUMN_CQZ, Qt::Horizontal, tr("CQ"));
    setHeaderData(COLUMN_ITUZ, Qt::Horizontal, tr("ITU"));
    setHeaderData(COLUMN_PREFIX, Qt::Horizontal, tr("Prefix"));
    setHeaderData(COLUMN_STATE, Qt::Horizontal, tr("State"));
    setHeaderData(COLUMN_COUNTY, Qt::Horizontal, tr("County"));
    setHeaderData(COLUMN_IOTA, Qt::Horizontal, tr("IOTA"));
    setHeaderData(COLUMN_QSL_RCVD, Qt::Horizontal, tr("QSL Rcvd"));
    setHeaderData(COLUMN_QSL_RCVD_DATE, Qt::Horizontal, tr("QSL Rcvd Date"));
    setHeaderData(COLUMN_QSL_SENT, Qt::Horizontal, tr("QSL Sent"));
    setHeaderData(COLUMN_QSL_SENT_DATE, Qt::Horizontal, tr("QSL Sent Date"));
    setHeaderData(COLUMN_LOTW_RCVD, Qt::Horizontal, tr("LotW Rcvd"));
    setHeaderData(COLUMN_LOTW_RCVD_DATE, Qt::Horizontal, tr("LotW Rcvd Date"));
    setHeaderData(COLUMN_LOTW_SENT, Qt::Horizontal, tr("LotW Sent"));
    setHeaderData(COLUMN_LOTW_SENT_DATE, Qt::Horizontal, tr("LotW Sent Date"));
    setHeaderData(COLUMN_TX_POWER, Qt::Horizontal, tr("TX Power"));
    setHeaderData(COLUMN_FIELDS, Qt::Horizontal, tr("Fields"));
}

QVariant LogbookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole && index.column() == COLUMN_CALL) {
        QModelIndex dxcc_index = this->index(index.row(), COLUMN_DXCC);
        int dxcc = QSqlTableModel::data(dxcc_index, Qt::DisplayRole).toInt();
        QString flag = Data::instance()->dxccFlag(dxcc);

        if (!flag.isEmpty()) {
            return QIcon(QString(":/flags/16/%1.png").arg(flag));
        }
        else {
            return QIcon(":/flags/16/unknown.png");
        }
    }

    if (role == Qt::DecorationRole && (index.column() == COLUMN_QSL_RCVD || index.column() == COLUMN_QSL_SENT ||
                                       index.column() == COLUMN_LOTW_RCVD || index.column() == COLUMN_LOTW_SENT))
    {
        QVariant value = QSqlTableModel::data(index, Qt::DisplayRole);
        if (value.toString() == "Y") {
            return QIcon(":/icons/done-24px.svg");
        }
//        else {
//            return QIcon(":/icons/close-24px.svg");
//        }
    }
    else {
        return QSqlTableModel::data(index, role);
    }
}

bool LogbookModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool main_update_result = true;
    bool depend_update_result = true;

    if ( role == Qt::EditRole )
    {
        switch ( index.column() )
        {
        case COLUMN_TIME_ON:
        {
            QDateTime time_on = QSqlTableModel::data(this->index(index.row(), COLUMN_TIME_ON), Qt::DisplayRole).toDateTime();
            QDateTime time_off = QSqlTableModel::data(this->index(index.row(), COLUMN_TIME_OFF), Qt::DisplayRole).toDateTime();
            qint64 diff = time_on.secsTo(time_off);

            depend_update_result = QSqlTableModel::setData(this->index(index.row(), COLUMN_TIME_OFF), QVariant(value.toDateTime().addSecs(diff)), role);
            break;
        }

        case COLUMN_TIME_OFF:
        {
            QDateTime time_on = QSqlTableModel::data(this->index(index.row(), COLUMN_TIME_ON), Qt::DisplayRole).toDateTime();

            if ( value.toDateTime() < time_on )
            {
                depend_update_result = false;
            }
            break;
        }

        case COLUMN_CALL:
        {
            bool ret1, ret2, ret3, ret4, ret5;
            QString new_callsign = value.toString();
            DxccEntity dxccEntity = Data::instance()->lookupDxcc(new_callsign);
            if ( dxccEntity.dxcc )
            {
                ret1 = QSqlTableModel::setData(this->index(index.row(), COLUMN_COUNTRY), QVariant(dxccEntity.country),role);
                ret2 = QSqlTableModel::setData(this->index(index.row(), COLUMN_CQZ), QVariant(dxccEntity.cqz),role);
                ret3 = QSqlTableModel::setData(this->index(index.row(), COLUMN_ITUZ), QVariant(dxccEntity.ituz),role);
                ret4 = QSqlTableModel::setData(this->index(index.row(), COLUMN_DXCC), QVariant(dxccEntity.dxcc),role);
                ret5 = QSqlTableModel::setData(this->index(index.row(), COLUMN_CONTINENT), QVariant(dxccEntity.cont),role);
            }
            else
            {
                ret1 = QSqlTableModel::setData(this->index(index.row(), COLUMN_COUNTRY), QVariant(QString()),role);
                ret2 = QSqlTableModel::setData(this->index(index.row(), COLUMN_CQZ), QVariant(QString()),role);
                ret3 = QSqlTableModel::setData(this->index(index.row(), COLUMN_ITUZ), QVariant(QString()),role);
                ret4 = QSqlTableModel::setData(this->index(index.row(), COLUMN_DXCC), QVariant(QString()),role);
                ret5 = QSqlTableModel::setData(this->index(index.row(), COLUMN_CONTINENT), QVariant(QString()),role);
            }
            depend_update_result = ret1 && ret2 && ret3 && ret4 && ret5;

            main_update_result = QSqlTableModel::setData(index, QVariant(value.toString().toUpper()), role);

            return main_update_result && depend_update_result;

            break;
        }

        case COLUMN_FREQUENCY:
        {
            depend_update_result = QSqlTableModel::setData(this->index(index.row(), COLUMN_BAND), QVariant(freqToBand(value.toDouble())), role );
            break;
        }

        case COLUMN_BAND:
        {
            depend_update_result = false;
            break;
        }

        case COLUMN_GRID:
        {
            depend_update_result = gridValidate(value.toString());
            break;
        }

        case COLUMN_ID:
        case COLUMN_DXCC:
        case COLUMN_COUNTRY:
        case COLUMN_CONTINENT:
        case COLUMN_CQZ:
        case COLUMN_ITUZ:
        {
            depend_update_result = false;
            break;
        }
        }
    }

    if ( depend_update_result )
    {
        main_update_result = QSqlTableModel::setData(index, value, role);
    }

    return main_update_result && depend_update_result;
}
