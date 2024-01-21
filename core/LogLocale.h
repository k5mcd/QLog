#ifndef QLOG_CORE_LOGLOCALE_H
#define QLOG_CORE_LOGLOCALE_H

#include <QLocale>

class LogLocale : public QLocale
{
public:
    LogLocale();
    QString formatTimeLongWithoutTZ() const;
    QString formatTimeShort() const;
    QString formatTimeLong() const;
    QString formatDateShortWithYYYY() const;
    QString formatDateTimeShortWithYYYY() const;
};

#endif // QLOG_CORE_LOGLOCALE_H
