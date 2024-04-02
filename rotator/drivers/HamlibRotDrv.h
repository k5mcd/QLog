#ifndef ROTATOR_DRIVERS_HAMLIBROTDRV_H
#define ROTATOR_DRIVERS_HAMLIBROTDRV_H

#include <QTimer>
#include <hamlib/rotator.h>
#include "GenericRotDrv.h"
#include "rotator/RotCaps.h"

class HamlibRotDrv : public GenericRotDrv
{
public:
    static QList<QPair<int, QString>> getModelList();
    static RotCaps getCaps(int model);

    explicit HamlibRotDrv(const RotProfile &profile,
                       QObject *parent = nullptr);

    virtual ~HamlibRotDrv();

    virtual bool open() override;
    virtual void sendState() override;
    virtual void setPosition(double azimuth, double elevation) override;
    virtual void stopTimers() override;

private slots:
    void checkRotStateChange();

private:
    static int addRig(const struct rot_caps* caps, void* data);

    void checkChanges();
    void checkAzEl();

    serial_handshake_e stringToHamlibFlowControl(const QString &in_flowcontrol);
    serial_parity_e stringToHamlibParity(const QString &in_parity);
    QString hamlibErrorString(int);
    void commandSleep();

    ROT* rot;
    QTimer timer;
    QMutex drvLock;

    bool forceSendState;
};

#endif // ROTATOR_DRIVERS_HAMLIBROTDRV_H
