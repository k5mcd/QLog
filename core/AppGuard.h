#ifndef APPGUARD_H
#define APPGUARD_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>

class AppGuard
{

public:
    AppGuard( const QString& key );
    ~AppGuard();

    bool isAnotherRunning(void);
    bool tryToRun(void);
    void release(void);

private:
    const QString key;
    const QString memLockKey;
    const QString sharedmemKey;

    QSharedMemory sharedMem;
    QSystemSemaphore memLock;

    Q_DISABLE_COPY( AppGuard )
};

#endif // APPGUARD_H
