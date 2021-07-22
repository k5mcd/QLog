#ifndef UTILS_H
#define UTILS_H

#include <QtCore>

bool gridValidate(QString grid);
bool gridToCoord(QString grid, double& lat, double& lon);
double coordDistance(double latA, double lonA, double latB, double lonB);
int coordBearing(double latA, double lonA, double latB, double lonB);
QString freqToBand(double freq);
int savePassword(QString storage_key, QString user, QString pass);
QString getPassword(QString storage_key, QString user);
void deletePassword(QString storage_key, QString user);

#endif // UTILS_H
