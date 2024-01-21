#ifndef QLOG_DATA_SOTAENTITY_H
#define QLOG_DATA_SOTAENTITY_H

#include <QtCore>

class SOTAEntity {
public:
    QString summitCode;
    QString associationName;
    QString regionName;
    QString summitName;
    qint32 altm;
    qint32 altft;
    double gridref1;
    double gridref2;
    double longitude;
    double latitude;
    qint16 points;
    qint16 bonusPoints;
    QDate validFrom;
    QDate validTo;
};

#endif // QLOG_DATA_SOTAENTITY_H
