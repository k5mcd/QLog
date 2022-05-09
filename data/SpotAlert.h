#ifndef SPOTALERT_H
#define SPOTALERT_H

#include <QString>
#include <QDateTime>
#include <QMetaType>
#include "Dxcc.h"

struct SpotAlert
{
    enum ALERTSOURCETYPE
    {
        DXSPOT = 0b1,
        WSJTXCQSPOT = 0b10
    };

    QDateTime dateTime;
    ALERTSOURCETYPE source;
    QStringList ruleName;
    QString callsign;
    double freq;
    QString band;
    QString mode;
    DxccEntity dxcc;
    DxccStatus status;
    QString comment;
    QString spotter;
    DxccEntity dxcc_spotter;
};

Q_DECLARE_METATYPE(SpotAlert::ALERTSOURCETYPE);

#endif // SPOTALERT_H
