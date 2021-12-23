#include <QEventLoop>
#include <qt5keychain/keychain.h>
#include "CredentialStore.h"
#include "core/debug.h"

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

int CredentialStore::savePassword(const QString &storage_key, const QString &user, const QString &pass)
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

    // write a password to Credential Storage
    WritePasswordJob job(QLatin1String(storage_key.toStdString().c_str()));
    job.setAutoDelete(false);
#ifdef Q_OS_WIN
    // see more qtkeychain issue #105
    user.prepend(storage_key + ":");
#endif
    job.setKey(user);
    job.setTextData(pass);

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

QString CredentialStore::getPassword(const QString &storage_key, const QString &user)
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

    // get a password from Credential Storage
    ReadPasswordJob job(QLatin1String(storage_key.toStdString().c_str()));
    job.setAutoDelete(false);
#ifdef Q_OS_WIN
    // see more qtkeychain issue #105
    user.prepend(storage_key + ":");
#endif
    job.setKey(user);

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

void CredentialStore::deletePassword(const QString &storage_key, const QString &user)
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

    // delete password from Secure Storage
    DeletePasswordJob job(QLatin1String(storage_key.toStdString().c_str()));
    job.setAutoDelete(false);
#ifdef Q_OS_WIN
    // see more qtkeychain issue #105
    user.prepend(storage_key + ":");
#endif
    job.setKey(user);

    job.connect(&job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));
    job.start();

    loop.exec();

    return;
}
