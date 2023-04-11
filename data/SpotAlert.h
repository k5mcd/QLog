#ifndef SPOTALERT_H
#define SPOTALERT_H

#include <QString>
#include <QDateTime>
#include <QMetaType>
#include "Dxcc.h"
#include "core/MembershipQE.h"

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
    QList<ClubInfo> callsign_member;
    QString mode;
    DxccEntity dxcc;
    DxccStatus status;
    QString comment;
    QString spotter;
    DxccEntity dxcc_spotter;

    QStringList memberList2StringList() const
    {
        QStringList ret;
        for ( const ClubInfo &member : qAsConst(callsign_member) )
        {
            ret << member.getClubInfo();
        }
        return ret;
    };
};

Q_DECLARE_METATYPE(SpotAlert::ALERTSOURCETYPE);

#endif // SPOTALERT_H
