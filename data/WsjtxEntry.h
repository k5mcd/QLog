#ifndef WSJTXENTRY_H
#define WSJTXENTRY_H

#include "core/Wsjtx.h"
#include "data/Data.h"
#include "core/MembershipQE.h"

struct WsjtxEntry {
    WsjtxDecode decode;
    DxccEntity dxcc;
    DxccStatus status;
    QString callsign;
    QList<ClubInfo> callsign_member;
    QString grid;
    double distance;
    QDateTime receivedTime;
    double freq;
    QString band;
    QString decodedMode;
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

#endif // WSJTXENTRY_H
