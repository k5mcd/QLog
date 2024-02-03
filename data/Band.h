#ifndef QLOG_DATA_BAND_H
#define QLOG_DATA_BAND_H

#include <QtCore>

class Band {
public:
    QString name;
    double start;
    double end;
    bool operator==(const Band &band)
    {
        return ( this->name == band.name
                 && this->start == band.start
                 && this->end == band.end );
    }
};

#endif // QLOG_DATA_BAND_H
