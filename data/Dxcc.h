#ifndef DXCC_H
#define DXCC_H

#include <QtCore>

enum DxccStatus {
    NewEntity     = 0b1,
    NewBand       = 0b10,
    NewMode       = 0b100,
    NewBandMode   = 0b110,
    NewSlot       = 0b1000,
    Worked        = 0b10000,
    Confirmed     = 0b100000,
    UnknownStatus = 0b1000000,
    All           = 0b1111111
};

class DxccEntity {
public:
    QString country;
    QString prefix;
    qint32 dxcc;
    QString cont;
    qint32 cqz;
    qint32 ituz;
    double latlon[2];
    float tz;
    QString flag;
};

struct DxccPrefix {
public:
    QString prefix;
    bool exact;
    qint32 dxcc;
    qint32 cqz;
    qint32 ituz;
    QString cont;
    double latlon[2];
};

#endif // DXCC_H
