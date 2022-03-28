#ifndef WSJTXENTRY_H
#define WSJTXENTRY_H

#include "core/Wsjtx.h"
#include "data/Data.h"

struct WsjtxEntry {
    WsjtxDecode decode;
    DxccEntity dxcc;
    DxccStatus status;
    QString callsign;
    QString grid;
    QDateTime receivedTime;
    double freq;
    QString band;
    QString decodedMode;
};

#endif // WSJTXENTRY_H