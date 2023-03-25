#ifndef DXSPOT_H
#define DXSPOT_H

#include <QtCore>
#include "Dxcc.h"
#include "core/MembershipQE.h"

class DxSpot {
public:
    QDateTime time;
    QString callsign;
    QList<ClubInfo> callsign_member;
    double freq;
    QString band;
    QString mode;
    QString spotter;
    QString comment;
    DxccEntity dxcc;
    DxccEntity dxcc_spotter;
    DxccStatus status;

    QStringList memberList2StringList() const
    {
        QStringList ret;
        for ( const ClubInfo &member : qAsConst(callsign_member) )
        {
            ret << member.getClubInfo();
        }
        return ret;
    };

    QSet<QString> memberList2Set() const
    {
        QSet<QString> ret;

        for ( const ClubInfo &member : qAsConst(callsign_member) )
        {
            ret << member.getClubInfo();
        }
        return ret;
    }



};

#endif // DXSPOT_H
