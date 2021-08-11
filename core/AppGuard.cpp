#include <QCryptographicHash>
#include <QLoggingCategory>
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
    , sharedMem( sharedmemKey )
    , memLock( memLockKey, 1 )
{
    FCT_IDENTIFICATION;
    memLock.acquire();
    {
        // linux / unix shared memory is not freed when the application terminates abnormally,
        // so you need to get rid of the garbage

        QSharedMemory fix( sharedmemKey );
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
