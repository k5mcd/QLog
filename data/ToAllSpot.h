#ifndef TOALLSPOT_H
#define TOALLSPOT_H

#include <QtGlobal>
#include "data/Dxcc.h"

class ToAllSpot {
public:
    QDateTime time;

    DxccEntity dxcc_spotter;
    QString spotter;
    QString message;
};

#endif // TOALLSPOT_H
