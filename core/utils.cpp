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
