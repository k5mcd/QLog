#include <QDir>
#include <QSqlQuery>
#include <QSqlError>

#include "QSLStorage.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.core.qslstorage");

QSLStorage::QSLStorage(QObject *parent) : QObject(parent)
{
    FCT_IDENTIFICATION;
}

bool QSLStorage::add(const QSLObject &qslObject)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << qslObject.getQSOID()
                                 << qslObject.getSource()
                                 << qslObject.getQSLName();
    QSqlQuery insert;

    if ( !insert.prepare("REPLACE INTO contacts_qsl_cards (contactid, source, name, data) "
                         " VALUES (:contactid, :source, :name, :data)" ) )
    {
        qCDebug(runtime) << " Cannot prepare INSERT for PaperQSL " << insert.lastError();
        return false;
    }

    insert.bindValue(":contactid", qslObject.getQSOID());
    insert.bindValue(":source", qslObject.getSource());
    insert.bindValue(":name", qslObject.getQSLName());
    insert.bindValue(":data", qslObject.getBLOB(QSLObject::BASE64FORM));

    if ( !insert.exec() )
    {
        qCDebug(runtime) << "Cannot import QSL" << insert.lastError();
        return false;
    }
    return true;
}

bool QSLStorage::remove(const QSqlRecord &qso,
                        const QSLObject::SourceType source,
                        const QString &qslName)
{
    FCT_IDENTIFICATION;

    QSqlQuery query;

    if ( !query.prepare("DELETE FROM contacts_qsl_cards "
                        "WHERE source = :source "
                        "AND contactid = :contactid "
                        "AND name = :qsl_name"))
    {
        qCDebug(runtime) << "Cannot prepare SQL Statement";
        return false;
    }

    query.bindValue(":source", source);
    query.bindValue(":contactid", qso.value("id"));
    query.bindValue(":qsl_name", qslName);

    if ( !query.exec() )
    {
        qCDebug(runtime) << "Cannot delete QSL file" << qslName;
        return false;
    }

    return true;
}

QStringList QSLStorage::getAvailableQSLNames(const QSqlRecord &qso,
                                             const QSLObject::SourceType sourceFilter) const
{
    FCT_IDENTIFICATION;

    QStringList ret;
    QSqlQuery query;

    if ( !query.prepare("SELECT name FROM contacts_qsl_cards "
                        "WHERE source = :source "
                        "AND contactid = :contactid "
                        "ORDER BY name"))
    {
        qCDebug(runtime) << "Cannot prepare SQL Statement";
        return ret;
    }

    query.bindValue(":source", sourceFilter);
    query.bindValue(":contactid", qso.value("id"));

    if ( query.exec() )
    {
        while(query.next())
        {
            ret << query.value(0).toString();
        }
    }
    else
    {
        qCDebug(runtime) << "Error" << query.lastError();
    }
    return ret;
}

QSLObject QSLStorage::getQSL(const QSqlRecord &qso,
                             const QSLObject::SourceType source,
                             const QString &qslName) const
{
    FCT_IDENTIFICATION;

    QSqlQuery query;

    if ( !query.prepare("SELECT data FROM contacts_qsl_cards "
                        "WHERE source = :source "
                        "AND contactid = :contactid "
                        "AND name = :qsl_name "
                        "ORDER BY name LIMIT 1"))
    {
        qCDebug(runtime) << "Cannot prepare SQL Statement";
    }
    else
    {
        query.bindValue(":source", source);
        query.bindValue(":contactid", qso.value("id"));
        query.bindValue(":qsl_name", qslName);

        if ( query.exec() && query.next() )
            return QSLObject(qso, source, qslName, query.value(0).toByteArray(), QSLObject::BASE64FORM);
    }

    return QSLObject (qso, source, qslName, QByteArray(), QSLObject::RAWBYTES);
}
