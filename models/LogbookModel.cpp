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
    setHeaderData(COLUMN_CQZ, Qt::Horizontal, tr("CQZ"));
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
    setHeaderData(COLUMN_ADDRESS, Qt::Horizontal, tr("Address"));
    setHeaderData(COLUMN_ADDRESS_INTL, Qt::Horizontal, tr("Address(Int)"));
    setHeaderData(COLUMN_AGE, Qt::Horizontal, tr("Age"));
    setHeaderData(COLUMN_A_INDEX, Qt::Horizontal, tr("A-Index"));
    setHeaderData(COLUMN_ANT_AZ, Qt::Horizontal, tr("Antenna Az"));
    setHeaderData(COLUMN_ANT_EL, Qt::Horizontal, tr("Antenna El"));
    setHeaderData(COLUMN_ANT_PATH, Qt::Horizontal, tr("Signal Path"));
    setHeaderData(COLUMN_ARRL_SECT, Qt::Horizontal, tr("ARRL Section"));
    setHeaderData(COLUMN_AWARD_SUBMITTED, Qt::Horizontal, tr("Award Submitted"));
    setHeaderData(COLUMN_AWARD_GRANTED, Qt::Horizontal, tr("Award Granted"));
    setHeaderData(COLUMN_BAND_RX, Qt::Horizontal, tr("Band RX"));
    setHeaderData(COLUMN_CHECK, Qt::Horizontal, tr("Contest Check"));
    setHeaderData(COLUMN_CLASS, Qt::Horizontal, tr("Class"));
    setHeaderData(COLUMN_CLUBLOG_QSO_UPLOAD_DATE, Qt::Horizontal, tr("ClubLog Upload Date"));
    setHeaderData(COLUMN_CLUBLOG_QSO_UPLOAD_STATUS, Qt::Horizontal, tr("ClubLog Upload State"));
    setHeaderData(COLUMN_COMMENT, Qt::Horizontal, tr("Comment"));
    setHeaderData(COLUMN_COMMENT_INTL, Qt::Horizontal, tr("Comment(Int)"));
    setHeaderData(COLUMN_CONTACTED_OP, Qt::Horizontal, tr("Contacted Operator"));
    setHeaderData(COLUMN_CONTEST_ID, Qt::Horizontal, tr("Contest ID"));
    setHeaderData(COLUMN_COUNTRY_INTL, Qt::Horizontal, tr("Country(Int)"));
    setHeaderData(COLUMN_CREDIT_SUBMITTED, Qt::Horizontal, tr("Credit Submitted"));
    setHeaderData(COLUMN_CREDIT_GRANTED, Qt::Horizontal, tr("Credit Granted"));
    setHeaderData(COLUMN_DARC_DOK, Qt::Horizontal, tr("DARC DOK"));
    setHeaderData(COLUMN_DISTANCE, Qt::Horizontal, tr("Dinstance"));
    setHeaderData(COLUMN_EMAIL, Qt::Horizontal, tr("email"));
    setHeaderData(COLUMN_EQ_CALL, Qt::Horizontal, tr("Owner Callsign"));
    setHeaderData(COLUMN_EQSL_QSLRDATE, Qt::Horizontal, tr("eQSL Rcvd Date"));
    setHeaderData(COLUMN_EQSL_QSLSDATE, Qt::Horizontal, tr("eQSL Sent Date"));
    setHeaderData(COLUMN_EQSL_QSL_RCVD, Qt::Horizontal, tr("eQSL Rcvd"));
    setHeaderData(COLUMN_EQSL_QSL_SENT, Qt::Horizontal, tr("eQSL Sent"));
    setHeaderData(COLUMN_FISTS, Qt::Horizontal, tr("FISTS Number"));
    setHeaderData(COLUMN_FISTS_CC, Qt::Horizontal, tr("FISTS CC"));
    setHeaderData(COLUMN_FORCE_INIT, Qt::Horizontal, tr("EME Init"));
    setHeaderData(COLUMN_FREQ_RX, Qt::Horizontal, tr("Frequency RX"));
    setHeaderData(COLUMN_GUEST_OP, Qt::Horizontal, tr("Guest Operator"));
    setHeaderData(COLUMN_HRDLOG_QSO_UPLOAD_DATE, Qt::Horizontal, tr("HRDLog Upload Date"));
    setHeaderData(COLUMN_HRDLOG_QSO_UPLOAD_STATUS, Qt::Horizontal, tr("HRDLog Upload Status"));
    setHeaderData(COLUMN_IOTA_ISLAND_ID, Qt::Horizontal, tr("IOTA"));
    setHeaderData(COLUMN_K_INDEX, Qt::Horizontal, tr("K-Index"));
    setHeaderData(COLUMN_LAT, Qt::Horizontal, tr("Latitude"));
    setHeaderData(COLUMN_LON, Qt::Horizontal, tr("Longitude"));
    setHeaderData(COLUMN_MAX_BURSTS, Qt::Horizontal, tr("Max Bursts"));
    setHeaderData(COLUMN_MS_SHOWER, Qt::Horizontal, tr("MS Shower Name"));
    setHeaderData(COLUMN_MY_ANTENNA, Qt::Horizontal, tr("My Antenna"));
    setHeaderData(COLUMN_MY_ANTENNA_INTL, Qt::Horizontal, tr("My Antenna(Int)"));
    setHeaderData(COLUMN_MY_CITY, Qt::Horizontal, tr("My City"));
    setHeaderData(COLUMN_MY_CITY_INTL, Qt::Horizontal, tr("My City(Int)"));
    setHeaderData(COLUMN_MY_CNTY, Qt::Horizontal, tr("My County"));
    setHeaderData(COLUMN_MY_COUNTRY, Qt::Horizontal, tr("My Country"));
    setHeaderData(COLUMN_MY_COUNTRY_INTL, Qt::Horizontal, tr("My Country(Int)"));
    setHeaderData(COLUMN_MY_CQ_ZONE, Qt::Horizontal, tr("My CQZ"));
    setHeaderData(COLUMN_MY_DXCC, Qt::Horizontal, tr("My DXCC"));
    setHeaderData(COLUMN_MY_FISTS, Qt::Horizontal, tr("My FISTS"));
    setHeaderData(COLUMN_MY_GRIDSQUARE, Qt::Horizontal, tr("My Gridsquare"));
    setHeaderData(COLUMN_MY_IOTA, Qt::Horizontal, tr("My IOTA"));
    setHeaderData(COLUMN_MY_IOTA_ISLAND_ID, Qt::Horizontal, tr("IOTA Island ID"));
    setHeaderData(COLUMN_MY_ITU_ZONE, Qt::Horizontal, tr("My ITU"));
    setHeaderData(COLUMN_MY_LAT, Qt::Horizontal, tr("My Latitude"));
    setHeaderData(COLUMN_MY_LON, Qt::Horizontal, tr("My Longitude"));
    setHeaderData(COLUMN_MY_NAME, Qt::Horizontal, tr("My Name"));
    setHeaderData(COLUMN_MY_NAME_INTL, Qt::Horizontal, tr("My Name(Int)"));
    setHeaderData(COLUMN_MY_POSTAL_CODE, Qt::Horizontal, tr("Postal Code"));
    setHeaderData(COLUMN_MY_POSTAL_CODE_INTL, Qt::Horizontal, tr("Postal Code(Int)"));
    setHeaderData(COLUMN_MY_RIG, Qt::Horizontal, tr("My Rig"));
    setHeaderData(COLUMN_MY_RIG_INTL, Qt::Horizontal, tr("My Rig(Int)"));
    setHeaderData(COLUMN_MY_SIG, Qt::Horizontal, tr("My Special Interest Activity"));
    setHeaderData(COLUMN_MY_SIG_INTL, Qt::Horizontal, tr("My Special Interest Activity(Int)"));
    setHeaderData(COLUMN_MY_SIG_INFO, Qt::Horizontal, tr("My Spec. Interes Activity Info"));
    setHeaderData(COLUMN_MY_SIG_INFO_INTL, Qt::Horizontal, tr("My Spec. Interest Activity Info(Int)"));
    setHeaderData(COLUMN_MY_SOTA_REF, Qt::Horizontal, tr("My SOTA"));
    setHeaderData(COLUMN_MY_STATE, Qt::Horizontal, tr("My State"));
    setHeaderData(COLUMN_MY_STREET, Qt::Horizontal, tr("My Street"));
    setHeaderData(COLUMN_MY_STREET_INTL, Qt::Horizontal, tr("My Street(Int)"));
    setHeaderData(COLUMN_MY_USACA_COUNTIES, Qt::Horizontal, tr("My USA-CA Counties"));
    setHeaderData(COLUMN_MY_VUCC_GRIDS, Qt::Horizontal, tr("My VUCC Grids"));
    setHeaderData(COLUMN_NAME_INTL, Qt::Horizontal, tr("Operator Name"));
    setHeaderData(COLUMN_NOTES, Qt::Horizontal, tr("Notes"));
    setHeaderData(COLUMN_NOTES_INTL, Qt::Horizontal, tr("Notes(Int)"));
    setHeaderData(COLUMN_NR_BURSTS, Qt::Horizontal, tr("#MS Bursts"));
    setHeaderData(COLUMN_NR_PINGS, Qt::Horizontal, tr("#MS Pings"));
    setHeaderData(COLUMN_OPERATOR, Qt::Horizontal, tr("Logging Operator"));
    setHeaderData(COLUMN_OWNER_CALLSIGN, Qt::Horizontal, tr("Owner Callsign"));
    setHeaderData(COLUMN_PRECEDENCE, Qt::Horizontal, tr("Contest Precedence"));
    setHeaderData(COLUMN_PROP_MODE, Qt::Horizontal, tr("Propagation Mode"));
    setHeaderData(COLUMN_PUBLIC_KEY, Qt::Horizontal, tr("Public Encryption Key"));
    setHeaderData(COLUMN_QRZCOM_QSO_UPLOAD_DATE, Qt::Horizontal, tr("QRZ Upload Date"));
    setHeaderData(COLUMN_QRZCOM_QSO_UPLOAD_STATUS, Qt::Horizontal, tr("QRZ Upload Status"));
    setHeaderData(COLUMN_QSLMSG, Qt::Horizontal, tr("QSL Message"));
    setHeaderData(COLUMN_QSLMSG_INTL, Qt::Horizontal, tr("QSL Message(Int)"));
    setHeaderData(COLUMN_QSL_RCVD_VIA, Qt::Horizontal, tr("QSL Rcvd Via"));
    setHeaderData(COLUMN_QSL_SENT_VIA, Qt::Horizontal, tr("QSL Sent Via"));
    setHeaderData(COLUMN_QSL_VIA, Qt::Horizontal, tr("QSL Via"));
    setHeaderData(COLUMN_QSO_COMPLETE, Qt::Horizontal, tr("QSO Completed"));
    setHeaderData(COLUMN_QSO_RANDOM, Qt::Horizontal, tr("QSO Random"));
    setHeaderData(COLUMN_QTH_INTL, Qt::Horizontal, tr("QTH(Int)"));
    setHeaderData(COLUMN_REGION, Qt::Horizontal, tr("Regio"));
    setHeaderData(COLUMN_RIG, Qt::Horizontal, tr("Rig"));
    setHeaderData(COLUMN_RIG_INTL, Qt::Horizontal, tr("Rig(Int)"));
    setHeaderData(COLUMN_RX_PWR, Qt::Horizontal, tr("Contact PWR"));
    setHeaderData(COLUMN_SAT_MODE, Qt::Horizontal, tr("SAT Mode"));
    setHeaderData(COLUMN_SAT_NAME, Qt::Horizontal, tr("SAT Name"));
    setHeaderData(COLUMN_SFI, Qt::Horizontal, tr("Solar Flux"));
    setHeaderData(COLUMN_SIG, Qt::Horizontal, tr("Special Activity Group"));
    setHeaderData(COLUMN_SIG_INTL, Qt::Horizontal, tr("Special Activity Group(Int)"));
    setHeaderData(COLUMN_SIG_INFO, Qt::Horizontal, tr("Special Activity Group Info"));
    setHeaderData(COLUMN_SIG_INFO_INTL, Qt::Horizontal, tr("Special Activity Group Info(Int)"));
    setHeaderData(COLUMN_SILENT_KEY, Qt::Horizontal, tr("Silent Key"));
    setHeaderData(COLUMN_SKCC, Qt::Horizontal, tr("SKCC Member"));
    setHeaderData(COLUMN_SOTA_REF, Qt::Horizontal, tr("SOTA"));
    setHeaderData(COLUMN_SRX, Qt::Horizontal, tr("Contest Serial Number RX"));
    setHeaderData(COLUMN_SRX_STRING, Qt::Horizontal, tr("Contest Exchange RX"));
    setHeaderData(COLUMN_STATION_CALLSIGN, Qt::Horizontal, tr("Logging Station Callsign"));
    setHeaderData(COLUMN_STX, Qt::Horizontal, tr("Contest Serial Number TX"));
    setHeaderData(COLUMN_STX_STRING, Qt::Horizontal, tr("Contest Exchange TX"));
    setHeaderData(COLUMN_SWL, Qt::Horizontal, tr("SWL"));
    setHeaderData(COLUMN_TEN_TEN, Qt::Horizontal, tr("Ten-Ten Number"));
    setHeaderData(COLUMN_UKSMG, Qt::Horizontal, tr("UKSMG Member"));
    setHeaderData(COLUMN_USACA_COUNTIES, Qt::Horizontal, tr("USA-CA Counties"));
    setHeaderData(COLUMN_VE_PROV, Qt::Horizontal, tr("VE Prov"));
    setHeaderData(COLUMN_VUCC_GRIDS, Qt::Horizontal, tr("VUCC Grids"));
    setHeaderData(COLUMN_WEB, Qt::Horizontal, tr("Web URL"));
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
