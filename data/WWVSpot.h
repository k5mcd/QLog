#ifndef QLOG_DATA_WWVSPOT_H
#define QLOG_DATA_WWVSPOT_H

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

#endif // QLOG_DATA_WWVSPOT_H
