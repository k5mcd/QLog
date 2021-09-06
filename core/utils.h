#ifndef UTILS_H
#define UTILS_H

#include <QtCore>

QString freqToBand(double freq);
int savePassword(QString storage_key, QString user, QString pass);
QString getPassword(QString storage_key, QString user);
void deletePassword(QString storage_key, QString user);

#endif // UTILS_H
