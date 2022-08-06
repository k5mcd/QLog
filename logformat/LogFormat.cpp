#include <QtSql>
#include "LogFormat.h"
#include "AdiFormat.h"
#include "AdxFormat.h"
#include "JsonFormat.h"
#include "data/Data.h"
#include "core/debug.h"
#include "core/Gridsquare.h"

MODULE_IDENTIFICATION("qlog.logformat.logformat");

LogFormat::LogFormat(QTextStream& stream) : QObject(nullptr), stream(stream), duplicateQSOFunc(nullptr) {
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

void LogFormat::setDuplicateQSOCallback(duplicateQSOBehaviour (*func)(QSqlRecord *, QSqlRecord *))
{
    FCT_IDENTIFICATION;

    duplicateQSOFunc = func;
}

int LogFormat::runImport()
{
    FCT_IDENTIFICATION;

    this->importStart();

    int count = 0;
    int processedRec = 0;

    QSqlTableModel model;
    model.setTable("contacts");
    model.removeColumn(model.fieldIndex("id"));
    QSqlRecord record = model.record(0);
    duplicateQSOBehaviour dupSetting = LogFormat::ASK_NEXT;

    while (true)
    {
        record.clearValues();

        if (!this->importNext(record)) break;

        processedRec++;

        if ( processedRec % 10 == 0)
        {
            emit importPosition(stream.pos());
        }

        if ( dateRangeSet() )
        {
            if (!inDateRange(record.value("start_time").toDateTime().date()))
            {
                continue;
            }
        }

        if ( dupSetting != ACCEPT_ALL )
        {
            /* checking matching fields if they are not empty */
            if ( ! record.value("start_time").toDateTime().isValid()
                 || record.value("callsign").toString().isEmpty()
                 || record.value("band").toString().isEmpty()
                 || record.value("mode").toString().isEmpty() )
            {
                qCDebug(runtime) << "missing matching field";
                qCDebug(runtime) << record;
                continue;
            }

            QString matchFilter = QString("callsign='%1' AND mode=upper('%2') AND band=lower('%3') AND ABS(JULIANDAY(start_time)-JULIANDAY(datetime('%4')))*24<1")
                    .arg(record.value("callsign").toString(),
                         record.value("mode").toString(),
                         record.value("band").toString(),
                         record.value("start_time").toDateTime().toTimeSpec(Qt::UTC).toString("yyyy-MM-dd hh:mm:ss"));

            /* set filter */
            model.setFilter(matchFilter);
            model.select();

            if ( model.rowCount() > 0 )
            {
                if ( dupSetting == SKIP_ALL)
                {
                    continue;
                }

                /* Duplicate QSO found */
                if ( duplicateQSOFunc )
                {
                    QSqlRecord originalRecord = model.record(0);
                    dupSetting = duplicateQSOFunc(&record, &originalRecord);
                }

                switch ( dupSetting )
                {
                case ACCEPT_ALL:
                case ACCEPT_ONE:
                case ASK_NEXT:
                    break;

                case SKIP_ONE:
                case SKIP_ALL:
                    continue;
                    break;
                }
            }
        }

        DxccEntity entity = Data::instance()->lookupDxcc(record.value("callsign").toString());

        if ( (record.value("dxcc").isNull()
              || updateDxcc) && entity.dxcc)
        {
            record.setValue("dxcc", entity.dxcc);
            record.setValue("country", Data::removeAccents(entity.country));
            record.setValue("country_intl", entity.country);
        }

        if ( record.value("cont").isNull() && entity.dxcc )
        {
            record.setValue("cont", entity.cont);
        }

        if ( record.value("ituz").isNull() && entity.dxcc )
        {
            record.setValue("ituz", QString::number(entity.ituz));
        }

        if ( record.value("cqz").isNull() && entity.dxcc )
        {
            record.setValue("cqz", QString::number(entity.cqz));
        }

        if (record.value("band").isNull()
            && !record.value("frequency").isNull() )
        {
            double freq = record.value("frequency").toDouble();
            record.setValue("band", Data::band(freq).name);
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

        count++;
    }

    emit importPosition(stream.pos());
    emit finished(count);

    model.submitAll();

    this->importEnd();

    return count;
}

void LogFormat::runQSLImport(QSLFrom fromService)
{
    FCT_IDENTIFICATION;

    QSLMergeStat stats = {QStringList(), QStringList(), 0, 0, 0, 0};

    this->importStart();

    QSqlTableModel model;
    model.setTable("contacts");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord QSLRecord = model.record(0);

    while ( true )
    {
        QSLRecord.clearValues();

        if ( !this->importNext(QSLRecord) ) break;

        stats.qsos_checked++;

        if ( stats.qsos_checked % 10 == 0 )
        {
            emit importPosition(stream.pos());
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
                .arg(QSLRecord.value("callsign").toString(),
                     QSLRecord.value("mode").toString(),
                     QSLRecord.value("band").toString(),
                     QSLRecord.value("start_time").toDateTime().toTimeSpec(Qt::UTC).toString("yyyy-MM-dd hh:mm:ss"));

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
            /* https://lotw.arrl.org/lotw-help/developer-query-qsos-qsls/?lang=en */
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

                    originalRecord.setValue("qsl_rcvd_via", "E");

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

        case EQSL:
        {
            /* http://www.eqsl.cc/qslcard/DownloadInBox.txt */
            /*   CALL
                 QSO_DATE
                 TIME_ON
                 BAND
                 MODE
                 SUBMODE (tag only present if non-blank)
                 PROP_MODE (tag only present if non-blank)
                 RST_SENT (will be the sender's RST Sent, not yours)
                 RST_RCVD (we do not capture this in uploads, so will normally be 0 length)
                 QSL_SENT (always Y)
                 QSL_SENT_VIA (always E)
                 QSLMSG (if non-null and containing only valid printable ASCII characters)
                 QSLMSG_INTL (if non-null and containing international characters - see ADIF V3 specs)
                 APP_EQSL_SWL (tag only present if sender is SWL and then always Y)
                 APP_EQSL_AG (tag only present if sender has Authenticity Guaranteed status and then always Y)
                 GRIDSQUARE (tag only present if non-blank and at least 4 long)
            */

            if ( originalRecord.value("eqsl_qsl_rcvd").toString() != 'Y' )
            {
                originalRecord.setValue("eqsl_qsl_rcvd", QSLRecord.value("qsl_sent"));

                originalRecord.setValue("eqsl_qslrdate", QDateTime::currentDateTimeUtc().date().toString("yyyy-MM-dd"));

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

                originalRecord.setValue("qslmsg", QSLRecord.value("qslmsg"));

                originalRecord.setValue("qslmsg_int", QSLRecord.value("qslmsg_int"));

                originalRecord.setValue("qsl_rcvd_via", QSLRecord.value("qsl_sent_via"));

                model.setRecord(0, originalRecord);
                model.submitAll();
                stats.qsos_updated++;
                stats.newQSLs.append(QSLRecord.value("callsign").toString());
            }

            break;
        }

        default:
            qCDebug(runtime) << "Uknown QSL import";
        }
    }

    emit importPosition(stream.pos());

    this->importEnd();

    emit QSLMergeFinished(stats);
}

int LogFormat::runExport()
{
    FCT_IDENTIFICATION;

    this->exportStart();

    QSqlQuery query;
    if (dateRangeSet())
    {
        if ( ! query.prepare("SELECT * FROM contacts"
                      " WHERE (start_time BETWEEN :start_date AND :end_date)"
                      " ORDER BY start_time ASC") )
        {
            qWarning() << "Cannot prepare select statement";
            return 0;
        }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        query.bindValue(":start_date", startDate.startOfDay());
        query.bindValue(":end_date", endDate.endOfDay());
#else /* Due to ubuntu 20.04 where qt5.12 is present */
        query.bindValue(":start_date", QDateTime(startDate));
        query.bindValue(":end_date", QDateTime(endDate));
#endif
    }
    else {
        if ( ! query.prepare("SELECT * FROM contacts ORDER BY start_time ASC") )
        {
            qWarning() << "Cannot prepare select statement";
            return 0;
        }
    }

    if ( ! query.exec() )
    {
        qWarning() << "Cannot execute select statement" << query.lastError();
        return 0;
    }

    int count = 0;

    /* following 3 lines are a workaround - SQLite does not
     * return a correct value for QSqlQuery.size
     */
    int rows = (query.last()) ? query.at() + 1 : 0;
    query.first();
    query.previous();

    while (query.next())
    {
        QSqlRecord record = query.record();
        this->exportContact(record);
        count++;
        if (count % 10 == 0)
        {
            emit exportProgress((int)(count * 100 / rows));
        }
    }

    emit exportProgress(100);

    this->exportEnd();
    return count;
}

int LogFormat::runExport(const QList<QSqlRecord> &selectedQSOs)
{
    FCT_IDENTIFICATION;

    this->exportStart();

    int count = 0;
    for (const QSqlRecord &qso: selectedQSOs)
    {
        this->exportContact(qso);
        count++;
        if (count % 10 == 0)
        {
            emit exportProgress((int)(count * 100 / selectedQSOs.size()));
        }
    }

    emit exportProgress(100);

    emit finished(count);

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

