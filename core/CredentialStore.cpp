#include <QEventLoop>
#include <QTimer>
#include "CredentialStore.h"
#include "core/debug.h"

#ifdef Q_OS_WIN
#include <keychain.h>
#else
#include <qt5keychain/keychain.h>
#endif

MODULE_IDENTIFICATION("qlog.core.appguard");

CredentialStore::CredentialStore(QObject *parent) : QObject(parent)
{
    FCT_IDENTIFICATION;
}

CredentialStore* CredentialStore::instance()
{
    FCT_IDENTIFICATION;

    static CredentialStore instance;
    return &instance;
}

int CredentialStore::savePassword(QString storage_key, QString user, QString pass)
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

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    // write a password to Credential Storage
    WritePasswordJob job(QLatin1String(storage_key.toStdString().c_str()));
    job.setAutoDelete(false);
#ifdef Q_OS_WIN
    // see more qtkeychain issue #105
    user.prepend(storage_key + ":");
#endif
    job.setKey(user);
    job.setTextData(pass);

    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    job.connect(&job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));

    job.start();
    timer.start(5000);  // miliseconds
    loop.exec();

    if ( timer.isActive() )
    {
        timer.stop();
        if (job.error())
        {
            qWarning() << "Cannot save a password. Error " << job.errorString();
            return 1;
        }
    }
    else
    {
        // timeout
        disconnect(&job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));
        qWarning()<<"Secure Store error - timeout";
    }

    return 0;
}

QString CredentialStore::getPassword(QString storage_key, QString user)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << storage_key << " " << user;

    using namespace QKeychain;
    QString pass;

    if ( user.isEmpty()
         || storage_key.isEmpty() )
    {
        return QString();
    }

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    // get a password from Credential Storage
    ReadPasswordJob job(QLatin1String(storage_key.toStdString().c_str()));
    job.setAutoDelete(false);
#ifdef Q_OS_WIN
    // see more qtkeychain issue #105
    user.prepend(storage_key + ":");
#endif
    job.setKey(user);

    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    job.connect(&job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));

    job.start();
    timer.start(5000);  // miliseconds
    loop.exec();

    if ( timer.isActive() )
    {
        timer.stop();
        pass = job.textData();

        if ( job.error() )
        {
            qWarning() << "Cannot get a password. Error " << job.errorString();
            return QString();
        }
    }
    else
    {
        // timeout
        disconnect(&job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));
        qWarning()<<"Secure Store error - timeout";
    }

    return pass;
}

void CredentialStore::deletePassword(QString storage_key, QString user)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << storage_key << " " << user;

    if ( user.isEmpty()
         || storage_key.isEmpty() )
    {
        return;
    }

    using namespace QKeychain;

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    // delete password from Secure Storage
    DeletePasswordJob job(QLatin1String(storage_key.toStdString().c_str()));
    job.setAutoDelete(false);
#ifdef Q_OS_WIN
    // see more qtkeychain issue #105
    user.prepend(storage_key + ":");
#endif
    job.setKey(user);

    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    job.connect(&job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));
    job.start();
    timer.start(5000);  // miliseconds
    loop.exec();

    if ( timer.isActive() )
    {
        timer.stop();
    }
    else
    {
        // timeout
        disconnect(&job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));
        qWarning()<<"Secure Store error - timeout";
    }
    return;
}
