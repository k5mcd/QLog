#ifndef GRIDSQUARE_H
#define GRIDSQUARE_H

#include <QObject>
#include <QString>
#include <QDebug>

class Gridsquare
{
public:
    explicit Gridsquare(QString in_grid = QString());
    ~Gridsquare() {};
    static QRegularExpression gridRegEx();
    static QRegularExpression gridVUCCRegEx();

    bool isValid();
    double getLongitude() {return lon;};
    double getLatitude() {return lat;};
    QString getGrid() { return grid;};
    bool distanceTo(Gridsquare in_grid, double &distance);
    bool distanceTo(double lat, double lon, double &distance);
    bool bearingTo(Gridsquare in_grid, double &bearing);
    bool bearingTo(double lat, double lon, double &bearing);
    operator QString() const { return QString("Gridsquare: grid[%1]; valid[%2]; lat[%3]; lon[%4]")
                                      .arg(grid).arg(validGrid).arg(lat).arg(lon);};

private:
    QString grid;
    bool validGrid;
    double lat, lon;
};

#endif // GRIDSQUARE_H
