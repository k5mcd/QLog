#include <QSqlQuery>
#include <QCache>

#include "LogParam.h"
#include "debug.h"

MODULE_IDENTIFICATION("qlog.core.logparam");

LogParam::LogParam(QObject *parent) :
    QObject(parent)
{
    FCT_IDENTIFICATION;
}

bool LogParam::setParam(const QString &name, const QString &value)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << name << " " << value;

    QSqlQuery query;

    if ( ! query.prepare("INSERT INTO log_param (name, value) "
                         "VALUES (:nam, :val)") )
    {
        qWarning()<< "Cannot prepare insert parameter statement";
        return false;
    }

    query.bindValue(":nam", name);
    query.bindValue(":val", value);

    if ( !query.exec() )
    {
        qWarning() << "Cannot exec an insert parameter statement";
        return false;
    }

    return true;
}

QString LogParam::getParam(const QString &name)
{
    FCT_IDENTIFICATION;

    static QCache<QString, QString> localCache(10);

    qCDebug(function_parameters) << name;

    QString ret;
    QString *valueCached = localCache.object(name);

    if ( valueCached )
    {
        ret = *valueCached;
    }
    else
    {

        QSqlQuery query;

        if ( ! query.prepare("SELECT value "
                             "FROM log_param "
                             "WHERE name = :nam") )
        {
            qWarning()<< "Cannot prepare insert parameter statement";
            return QString();
        }

        query.bindValue(":nam", name);

        if ( ! query.exec() )
        {
            qWarning() << "Cannot execute an get Parameter";
            return QString();
        }

        query.next();
        ret = query.value(0).toString();
    }
    qDebug(runtime) << "value: " << ret;
    return ret;
}
