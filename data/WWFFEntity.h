#ifndef WWFFENTITY_H
#define WWFFENTITY_H

#include <QtCore>

class WWFFEntity {
public:
    QString reference;
    QString status;
    QString name;
    QString program;
    QString dxcc;
    QString state;
    QString county;
    QString continent;
    QString iota;
    QString iaruLocator;
    double latitude;
    double longitude;
    QString iucncat;
    QDate validFrom;
    QDate validTo;
};


#endif // WWFFENTITY_H
