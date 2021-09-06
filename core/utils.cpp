#include <QtCore/QRegExp>
#include <QDebug>
#include <cmath>
#include <qt5keychain/keychain.h>
#include <QEventLoop>
#include "debug.h"
#include "utils.h"

MODULE_IDENTIFICATION("qlog.core.utils");

#define EARTH_RADIUS 6371
#define EARTH_CIRCUM 40075

QString freqToBand(double freq) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << freq;

    if (freq <= 2.0 && freq >= 1.8) return "160m";
    else if (freq <= 3.8 && freq >= 3.5) return "80m";
    else if (freq <= 7.5 && freq >= 7.0) return "40m";
    else if (freq <= 10.150 && freq >= 10.1) return"30m";
    else if (freq <= 14.350 && freq >= 14.0) return "20m";
    else if (freq <= 18.168 && freq >= 18.068) return "17m";
    else if (freq <= 21.450 && freq >= 21.000) return "15m";
    else if (freq <= 24.990 && freq >= 24.890) return "12m";
    else if (freq <= 29.700 && freq >= 28.000) return "10m";
    else if (freq <= 52 && freq >= 50) return "6m";
    else if (freq <= 148 && freq >= 144) return "2m";
    else if (freq <= 440 && freq >= 430) return "70cm";
    else return QString();
}

int savePassword(QString storage_key, QString user, QString pass)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << storage_key << " " << user;

    using namespace QKeychain;

    if ( user.isEmpty()
         || storage_key.isEmpty()
         || pass.isEmpty() )
    {
       return 1;
    }

    // write a password to Secure Storage
    WritePasswordJob job(QLatin1String(storage_key.toStdString().c_str()));
    job.setAutoDelete(false);
    job.setKey(user);
    job.setTextData(pass);
    QEventLoop loop;
    job.connect(&job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));
    job.start();
    loop.exec();

    if (job.error())
    {
        qWarning() << "Cannot save a password. Error " << job.errorString();
        return 1;
    }

    return 0;
}

QString getPassword(QString storage_key, QString user)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << storage_key << " " << user;

    using namespace QKeychain;
    QString pass;

    // if password already loaded then return it immediately
    if ( user.isEmpty()
         || storage_key.isEmpty() )
    {
        return QString();
    }

    // get a password from Secure Storage
    ReadPasswordJob job(QLatin1String(storage_key.toStdString().c_str()));
    job.setAutoDelete(false);
    job.setKey(user);
    QEventLoop loop;
    job.connect(&job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));
    job.start();
    loop.exec();

    pass = job.textData();

    if ( job.error() )
    {
        qWarning() << "Cannot get a password. Error " << job.errorString();
        return QString();
    }

    return pass;
}

void deletePassword(QString storage_key, QString user)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << storage_key << " " << user;

    using namespace QKeychain;

    // delete password from Secure Storage
    DeletePasswordJob job(QLatin1String(storage_key.toStdString().c_str()));
    job.setAutoDelete(false);
    job.setKey(user);
    QEventLoop loop;
    job.connect(&job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));
    job.start();
    loop.exec();

    return;
}
