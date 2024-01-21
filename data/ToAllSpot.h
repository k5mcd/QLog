#ifndef QLOG_DATA_TOALLSPOT_H
#define QLOG_DATA_TOALLSPOT_H

#include <QtGlobal>
#include "data/Dxcc.h"

class ToAllSpot {
public:
    QDateTime time;

    DxccEntity dxcc_spotter;
    QString spotter;
    QString message;
};

#endif // QLOG_DATA_TOALLSPOT_H
