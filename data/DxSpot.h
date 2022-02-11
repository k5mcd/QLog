#ifndef DXSPOT_H
#define DXSPOT_H

#include <QtCore>
#include "Dxcc.h"

class DxSpot {
public:
    QDateTime time;
    QString callsign;
    double freq;
    QString band;
    QString mode;
    QString spotter;
    QString comment;
    DxccEntity dxcc;
    DxccEntity dxcc_spotter;
    DxccStatus status;
};


#endif // DXSPOT_H
