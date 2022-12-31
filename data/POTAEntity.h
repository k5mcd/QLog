#ifndef POTAENTITY_H
#define POTAENTITY_H

#include <QtCore>

class POTAEntity {
public:
    QString reference;
    QString name;
    bool active;
    qint16 entityID;
    QString locationDesc;
    double longitude;
    double latitude;
    QString grid;
};

#endif // POTAENTITY_H
