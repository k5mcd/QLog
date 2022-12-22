#ifndef BAND_H
#define BAND_H

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

#endif // BAND_H
