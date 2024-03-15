#ifndef ROTATOR_DRIVERS_PSTROTDRV_H
#define ROTATOR_DRIVERS_PSTROTDRV_H

#include <QUdpSocket>
#include <QHostInfo>
#include <QTimer>
#include "GenericRotDrv.h"
#include "rotator/RotCaps.h"

class PSTRotDrv : public GenericRotDrv
{
    Q_OBJECT

public:
    static QList<QPair<int, QString>> getModelList();
    static RotCaps getCaps(int);

    explicit PSTRotDrv(const RotProfile &profile,
                       QObject *parent = nullptr);

    virtual ~PSTRotDrv();

    virtual bool open() override;
    virtual void sendState() override;
    virtual void setPosition(double azimuth, double elevation) override;
    virtual void stopTimers() override;

private slots:
    void checkRotStateChange();

private:
    void commandSleep();
    void sendCommand(const QString& cmd);
    void readPendingDatagrams();

    bool forceSendState;

    QTimer refreshTimer;
    QTimer timeoutTimer;
    QUdpSocket receiveSocket;
    QMutex drvLock;
    QHostInfo hostInfo;
};

#endif // ROTATOR_DRIVERS_PSTROTDRV_H
