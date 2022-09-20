#ifndef WCYSPOT_H
#define WCYSPOT_H

#include <QtCore>

class WCYSpot {
public:
    QDateTime time;
    quint16 KIndex;
    quint16 expK;
    quint16 AIndex;
    quint16 RIndex;
    quint16 SFI;
    QString SA;
    QString GMF;
    QString Au;
};

#endif // WCYSPOT_H
