#ifndef LOGLOCALE_H
#define LOGLOCALE_H

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

#endif // LOGLOCALE_H
