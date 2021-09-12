#include <QtSql>
#include "LogFormat.h"
#include "AdiFormat.h"
#include "AdxFormat.h"
#include "JsonFormat.h"
#include "data/Data.h"
#include "core/debug.h"
#include "core/Gridsquare.h"

MODULE_IDENTIFICATION("qlog.logformat.logformat");

LogFormat::LogFormat(QTextStream& stream) : QObject(nullptr), stream(stream) {
    FCT_IDENTIFICATION;
    this->defaults = nullptr;
}

LogFormat::~LogFormat() {
    FCT_IDENTIFICATION;
}

LogFormat* LogFormat::open(QString type, QTextStream& stream) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<type;

    type = type.toLower();

    if (type == "adi") {
        return open(LogFormat::ADI, stream);
    }
    else if (type == "adx") {
        return open(LogFormat::ADX, stream);
    }
    else if (type == "json") {
        return open(LogFormat::JSON, stream);
    }
    else if (type == "cabrillo") {
        return open(LogFormat::JSON, stream);
    }
    else {
        return nullptr;
    }
}

LogFormat* LogFormat::open(LogFormat::Type type, QTextStream& stream) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<type;

    switch (type) {
    case LogFormat::ADI:
        return new AdiFormat(stream);

    case LogFormat::ADX:
        return new AdxFormat(stream);

    case LogFormat::JSON:
        return new JsonFormat(stream);

    case LogFormat::CABRILLO:
        return nullptr;

    default:
        return nullptr;
    }
}

void LogFormat::setDefaults(QMap<QString, QString>& defaults) {
    FCT_IDENTIFICATION;

    this->defaults = &defaults;
}

void LogFormat::setDateRange(QDate start, QDate end) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<start << " " << end;
    this->startDate = start;
    this->endDate = end;
}

void LogFormat::setUpdateDxcc(bool updateDxcc) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<updateDxcc;

    this->updateDxcc = updateDxcc;
}

void LogFormat::runImport() {
    FCT_IDENTIFICATION;

    this->importStart();

    int count = 0;

    QSqlTableModel model;
    model.setTable("contacts");
    model.removeColumn(model.fieldIndex("id"));
    QSqlRecord record = model.record();

    while (true) {
        record.clearValues();

        if (!this->importNext(record)) break;

        if (dateRangeSet()) {
            if (!inDateRange(record.value("start_time").toDateTime().date())) {
                continue;
            }
        }

        DxccEntity entity = Data::instance()->lookupDxcc(record.value("callsign").toString());

        if ((record.value("dxcc").isNull() || updateDxcc) && entity.dxcc) {
            record.setValue("dxcc", entity.dxcc);
            record.setValue("country", entity.country);
        }

        if (record.value("cont").isNull() && entity.dxcc) {
            record.setValue("cont", entity.cont);
        }

        if (record.value("ituz").isNull() && entity.dxcc) {
            record.setValue("ituz", QString::number(entity.ituz));
        }

        if (record.value("cqz").isNull() && entity.dxcc) {
            record.setValue("cqz", QString::number(entity.cqz));
        }

        if (record.value("band").isNull() && !record.value("frequency").isNull()) {
            double freq = record.value("frequency").toDouble();
            record.setValue("band", Data::freqToBand(freq));
        }

        QString gridsquare = record.value("gridsquare").toString();
        QString my_gridsquare = record.value("my_gridsquare").toString();

        if ( !gridsquare.isEmpty()
             && !my_gridsquare.isEmpty()
             && record.value("distance").toString().isEmpty() )
        {
            Gridsquare grid(gridsquare);
            Gridsquare my_grid(my_gridsquare);
            double distance;

            if ( my_grid.distanceTo(grid, distance) )
            {
                record.setValue("distance", distance);
            }
        }

        model.insertRecord(-1, record);

        if (count % 10 == 0) {
            emit progress(stream.pos());
        }

        count++;
    }

    model.submitAll();

    this->importEnd();

    emit finished(count);
}

void LogFormat::runQSLImport(QSLFrom fromService)
{
    FCT_IDENTIFICATION;

    QSLMergeStat stats = {QStringList(), QStringList(), 0, 0, 0, 0};

    this->importStart();

    QSqlTableModel model;
    model.setTable("contacts");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord QSLRecord = model.record();
    QString filterString;

    while ( true )
    {
        QSLRecord.clearValues();

        if ( !this->importNext(QSLRecord) ) break;

        stats.qsos_checked++;

        if ( stats.qsos_checked % 10 == 0 )
        {
            emit progress(stream.pos());
        }

        /* checking matching fields if they are not empty */
        if ( ! QSLRecord.value("start_time").toDateTime().isValid()
             || QSLRecord.value("callsign").toString().isEmpty()
             || QSLRecord.value("band").toString().isEmpty()
             || QSLRecord.value("mode").toString().isEmpty() )
        {
            qCDebug(runtime) << "missing matching field";
            qCDebug(runtime) << QSLRecord;
            stats.qsos_errors++;
            continue;
        }

        QString matchFilter = QString("callsign='%1' AND mode=upper('%2') AND band=lower('%3') AND ABS(JULIANDAY(start_time)-JULIANDAY(datetime('%4')))*24<1")
                .arg(QSLRecord.value("callsign").toString())
                .arg(QSLRecord.value("mode").toString())
                .arg(QSLRecord.value("band").toString())
                .arg(QSLRecord.value("start_time").toDateTime().toTimeSpec(Qt::UTC).toString("yyyy-MM-dd hh:mm:ss"));

        /* set filter */
        model.setFilter(matchFilter);
        model.select();

        if ( model.rowCount() != 1 )
        {
            stats.qsos_unmatched++;
            stats.unmatchedQSLs.append(QSLRecord.value("callsign").toString());
            continue;
        }

        /* we have one row for updating */
        /* lets update it */
        QSqlRecord originalRecord = model.record(0);

        switch ( fromService )
        {
        case LOTW:
        {
            if ( !QSLRecord.value("lotw_qsl_rcvd").toString().isEmpty() )
            {
                if ( QSLRecord.value("qsl_rcvd") != originalRecord.value("lotw_qsl_rcvd")
                     && QSLRecord.value("qsl_rcvd").toString() == 'Y' )
                {
                    originalRecord.setValue("lotw_qsl_rcvd", QSLRecord.value("qsl_rcvd"));

                    originalRecord.setValue("lotw_qslrdate", QSLRecord.value("qsl_rdate"));

                    Gridsquare dxNewGrid(QSLRecord.value("gridsquare").toString());

                    if ( dxNewGrid.isValid()
                         && ( originalRecord.value("gridsquare").toString().isEmpty()
                              ||
                              dxNewGrid.getGrid().contains(originalRecord.value("gridsquare").toString()))
                       )
                    {
                        Gridsquare myGrid(originalRecord.value("my_gridsquare").toString());

                        originalRecord.setValue("gridsquare", dxNewGrid.getGrid());

                        double distance;

                        if ( myGrid.distanceTo(dxNewGrid, distance) )
                        {
                            originalRecord.setValue("distance", QVariant(distance));
                        }
                    }

                    if ( !QSLRecord.value("credit_granted").toString().isEmpty() )
                    {
                        originalRecord.setValue("credit_granted", QSLRecord.value("credit_granted"));
                    }

                    if ( !QSLRecord.value("credit_submitted").toString().isEmpty() )
                    {
                        originalRecord.setValue("credit_submitted", QSLRecord.value("credit_submitted"));
                    }

                    if ( !QSLRecord.value("pfx").toString().isEmpty() )
                    {
                        originalRecord.setValue("pfx", QSLRecord.value("pfx"));
                    }

                    if ( !QSLRecord.value("iota").toString().isEmpty() )
                    {
                        originalRecord.setValue("iota", QSLRecord.value("iota"));
                    }

                    if ( !QSLRecord.value("vucc_grids").toString().isEmpty() )
                    {
                        originalRecord.setValue("vucc_grids", QSLRecord.value("vucc_grids"));
                    }

                    if ( !QSLRecord.value("state").toString().isEmpty() )
                    {
                        originalRecord.setValue("state", QSLRecord.value("state"));
                    }

                    if ( !QSLRecord.value("cnty").toString().isEmpty() )
                    {
                        originalRecord.setValue("cnty", QSLRecord.value("cnty"));
                    }

                    model.setRecord(0, originalRecord);
                    model.submitAll();
                    stats.qsos_updated++;
                    stats.newQSLs.append(QSLRecord.value("callsign").toString());
                }
            }
            else
            {
                qCInfo(runtime) << "Malformed Lotw Record " << QSLRecord;
            }
            break;
        }
        default:
            qCDebug(runtime) << "Uknown QSL import";
        }
    }

    this->importEnd();

    emit QSLMergeFinished(stats);
}

int LogFormat::runExport() {
    FCT_IDENTIFICATION;

    this->exportStart();

    QSqlQuery query;
    if (dateRangeSet()) {
        query.prepare("SELECT * FROM contacts"
                      " WHERE (start_time BETWEEN :start_date AND :end_date)"
                      " ORDER BY start_time ASC");
        query.bindValue(":start_date", QDateTime(startDate));
        query.bindValue(":end_date", QDateTime(endDate));
    }
    else {
        query.prepare("SELECT * FROM contacts ORDER BY start_time ASC");
    }

    query.exec();

    int count = 0;
    while (query.next()) {
        QSqlRecord record = query.record();
        this->exportContact(record);
        count++;
    }

    this->exportEnd();
    return count;
}

bool LogFormat::dateRangeSet() {
    FCT_IDENTIFICATION;

    return !startDate.isNull() && !endDate.isNull();
}

bool LogFormat::inDateRange(QDate date) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<date;

    return date >= startDate && date <= endDate;
}

