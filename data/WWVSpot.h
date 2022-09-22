#ifndef WWVSPOT_H
#define WWVSPOT_H

#include <QtCore>

class WWVSpot {
public:
    QDateTime time;
    quint16 SFI;
    quint16 AIndex;
    quint16 KIndex;
    QString info1;
    QString info2;
};

#endif // WWVSPOT_H
