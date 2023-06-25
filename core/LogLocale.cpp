#include "LogLocale.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.core.loglocale");

LogLocale::LogLocale()
{
    FCT_IDENTIFICATION;
}

QString LogLocale::formatTimeLongWithoutTZ() const
{
    FCT_IDENTIFICATION;

    QString ret = timeFormat(QLocale::LongFormat).remove(" t");

    qCDebug(runtime) << "format:" << ret;
    return ret;
}

QString LogLocale::formatTimeShort() const
{
    FCT_IDENTIFICATION;

    QString ret = timeFormat(QLocale::ShortFormat);

    qCDebug(runtime) << "format:" << ret;
    return ret;
}

QString LogLocale::formatTimeLong() const
{
    FCT_IDENTIFICATION;

    QString ret = timeFormat(QLocale::LongFormat);

    qCDebug(runtime) << "format:" << ret;
    return ret;
}

QString LogLocale::formatDateShortWithYYYY() const
{
    FCT_IDENTIFICATION;

    QString ret = dateFormat(QLocale::ShortFormat);

    if ( ret.contains("yy") && !ret.contains("yyyy") )
        ret = ret.replace("yy", "yyyy");

    qCDebug(runtime) << "format:" << ret;
    return ret;
}

QString LogLocale::formatDateTimeShortWithYYYY() const
{
    FCT_IDENTIFICATION;

    QString ret = formatDateShortWithYYYY() + " " +formatTimeShort();

    qCDebug(runtime) << "format:" << ret;
    return ret;

}
