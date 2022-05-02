#ifndef USERALERT_H
#define USERALERT_H

#include <QString>
#include <QDateTime>
#include <QMetaType>
#include "Dxcc.h"

struct UserAlert
{
    enum ALERTSOURCETYPE
    {
        DXSPOT = 0,
        WSJTXCQSPOT = 1
    };

    QDateTime dateTime;
    ALERTSOURCETYPE source;
    QString triggerName;
    QString callsign;
    double freq;
    QString mode;
    DxccEntity dxcc;
    DxccStatus status;
    QString comment;
    QString spotter;
    DxccEntity dxcc_spotter;
};

Q_DECLARE_METATYPE(UserAlert::ALERTSOURCETYPE);

#endif // USERALERT_H
