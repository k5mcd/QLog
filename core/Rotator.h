#ifndef QLOG_CORE_ROTATOR_H
#define QLOG_CORE_ROTATOR_H

#include <QtCore>
#include <hamlib/rotator.h>
#include "data/RotProfile.h"
#include "core/SerialPort.h"

struct rot;

class Rotator : public SerialPort
{
    Q_OBJECT

public:
    static Rotator* instance();
    double getAzimuth();
    double getElevation();
    bool isRotConnected();

signals:
    void positionChanged(double azimuth, double elevation);
    void rotErrorPresent(QString, QString);
    void rotDisconnected();
    void rotConnected();

public slots:
    void start();
    void update();
    void open();
    void close();
    void stopTimer();
    void sendState();

    void setPosition(double azimuth, double elevation);

private slots:
    void setPositionImpl(double azimuth, double elevation);
    void stopTimerImplt();
    void openImpl();
    void closeImpl();

private:
    Rotator(QObject *parent = nullptr);
    ~Rotator();

    void __closeRot();
    void __openRot();
    QString hamlibErrorString(int);

    double azimuth;
    double elevation;

    ROT* rot;
    RotProfile connectedRotProfile;
    QMutex rotLock;
    QTimer* timer;

    bool forceSendState;
};

#endif // QLOG_CORE_ROTATOR_H
