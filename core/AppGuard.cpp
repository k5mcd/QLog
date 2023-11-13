#include <QCryptographicHash>
#include <QLoggingCategory>
#if QT_VERSION >= QT_VERSION_CHECK(6,6,0)
#include <QNativeIpcKey>
#endif
#include "AppGuard.h"
#include "debug.h"

MODULE_IDENTIFICATION("qlog.core.appguard");
namespace
{

    QString generateKeyHash( const QString& key, const QString& salt )
    {
        QByteArray data;

        data.append( key.toUtf8() );
        data.append( salt.toUtf8() );
        data = QCryptographicHash::hash( data, QCryptographicHash::Sha1 ).toHex();

        return data;
    }

}

AppGuard::AppGuard( const QString& key )
    : key( key )
    , memLockKey( generateKeyHash( key, "_memLockKey" ) )
    , sharedmemKey( generateKeyHash( key, "_sharedmemKey" ) )
    , sharedMem(
#if QT_VERSION >= QT_VERSION_CHECK(6,6,0)
          QSharedMemory::legacyNativeKey(sharedmemKey)
#else
          sharedmemKey
#endif
 )
    , memLock(
#if QT_VERSION >= QT_VERSION_CHECK(6,6,0)
          QSystemSemaphore::legacyNativeKey(memLockKey),
#else
          memLockKey,
#endif
     1 )
{
    FCT_IDENTIFICATION;

    memLock.acquire();
    {
        // linux / unix shared memory is not freed when the application terminates abnormally,
        // so you need to get rid of the garbage

        QSharedMemory fix(
#if QT_VERSION >= QT_VERSION_CHECK(6,6,0)
            QSharedMemory::legacyNativeKey(sharedmemKey)
#else
            sharedmemKey
#endif
                         );
        if ( fix.attach() )
        {
            fix.detach();
        }
    }
    memLock.release();
}

AppGuard::~AppGuard()
{
    release();
}

bool AppGuard::isAnotherRunning(void)
{
    FCT_IDENTIFICATION;

    if ( sharedMem.isAttached() )
        return false;

    memLock.acquire();
    const bool isRunning = sharedMem.attach();

    if ( isRunning )
    {
        sharedMem.detach();
    }

    memLock.release();

    return isRunning;
}

bool AppGuard::tryToRun(void)
{
    FCT_IDENTIFICATION;

    if ( isAnotherRunning() )   // Extra check
    {
        return false;
    }

    memLock.acquire();

    // The following 'attach' call is a workaround required for 'create' to run properly under QT 6.6 and MacOS.
    // See more details about it in issue #257.
    // It has no impact on other platforms and versions of Qt, therefore there is no compilation condition.
    sharedMem.attach();

    const bool result = sharedMem.create( sizeof( quint64 ) );
    memLock.release();
    if ( !result )
    {
        release();
        return false;
    }

    return true;
}

void AppGuard::release(void)
{
    FCT_IDENTIFICATION;

    memLock.acquire();

    if ( sharedMem.isAttached() )
    {
        sharedMem.detach();
    }

    memLock.release();
}
