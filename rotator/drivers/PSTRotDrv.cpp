#include <QNetworkDatagram>
#include "core/debug.h"
#include "PSTRotDrv.h"

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <unistd.h>
#endif

#define MUTEXLOCKER     qCDebug(runtime) << "Waiting for Rot Drv mutex"; \
                        QMutexLocker locker(&drvLock); \
                        qCDebug(runtime) << "Using Rot Drv"

#define POOL_INTERVAL 1000
#define COMMAND_TIMEOUT POOL_INTERVAL * 0.7

MODULE_IDENTIFICATION("qlog.rotator.driver.pstrotdrv");

QList<QPair<int, QString> > PSTRotDrv::getModelList()
{
     FCT_IDENTIFICATION;

     QList<QPair<int, QString>> ret;

     ret << QPair<int, QString>(1, tr("Rot 1"));
     return ret;
}

RotCaps PSTRotDrv::getCaps(int)
{
    FCT_IDENTIFICATION;

    RotCaps ret;

    ret.isNetworkOnly = true;

    return ret;
}

PSTRotDrv::PSTRotDrv(const RotProfile &profile, QObject *parent)
    : GenericRotDrv{profile, parent},
      forceSendState(false)
{
    FCT_IDENTIFICATION;
}

PSTRotDrv::~PSTRotDrv()
{
    FCT_IDENTIFICATION;

    PSTRotDrv::stopTimers();
}

bool PSTRotDrv::open()
{
    FCT_IDENTIFICATION;

    MUTEXLOCKER;

    bool rc = receiveSocket.bind(rotProfile.netport + 1);

    if ( !rc )
    {
        lastErrorText = tr("Initialization Error");
        qCDebug(runtime) << "Rot is not initialized - cannot bind port address" << rotProfile.netport;
        return false;
    }

    qCDebug(runtime) << "Listening port" << rotProfile.netport + 1;

    hostInfo = QHostInfo::fromName(rotProfile.hostname);

    if ( hostInfo.error() != QHostInfo::NoError )
    {
        lastErrorText = tr("Initialization Error");
        qCWarning(runtime) << "Cannot get Rotator hostname" << hostInfo.errorString();
        return false;
    }

    if ( hostInfo.addresses().size() < 1 )
    {
        lastErrorText = tr("Initialization Error");
        qCWarning(runtime) << "No IP Address for " << rotProfile.hostname;
        return false;
    }

    qCDebug(runtime) << hostInfo.addresses();

    connect(&receiveSocket, &QUdpSocket::readyRead,
            this, &PSTRotDrv::readPendingDatagrams);

    connect(&refreshTimer, &QTimer::timeout,
            this, &PSTRotDrv::checkRotStateChange);

    connect(&timeoutTimer, &QTimer::timeout,
            this, [this]()
    {
        timeoutTimer.stop();
        qCWarning(runtime) << "Operation Timeout";
        emit errorOccured(tr("Error Occured"),
                          tr("Operation Timeout"));
    });

    refreshTimer.start(POOL_INTERVAL);
    emit rotIsReady();
    opened = true;

    return rc;
}

void PSTRotDrv::sendState()
{
    FCT_IDENTIFICATION;

    MUTEXLOCKER;

    forceSendState = true;
}

void PSTRotDrv::setPosition(double in_azimuth, double in_elevation)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << in_azimuth << in_elevation;

    MUTEXLOCKER;

   if ( !opened )
       return;

   QString positionCommand = QString("<PST>"
                                     "<TRACK>0</TRACK>"
                                     "<AZIMUTH>%1</AZIMUTH>"
                                     "<ELEVATION>%2</ELEVATION>"
                                     "</PST>").arg(in_azimuth, 0, 'f', 1)
                                              .arg(in_elevation, 0, 'f', 1);

   sendCommand(positionCommand);
}

void PSTRotDrv::stopTimers()
{
    FCT_IDENTIFICATION;

    receiveSocket.close();
    refreshTimer.stop();
    timeoutTimer.stop();
}

void PSTRotDrv::checkRotStateChange()
{
    FCT_IDENTIFICATION;

    MUTEXLOCKER;

    QUdpSocket sendSocket;

    QString azCommand = "<PST>AZ?</PST>";
    QString elCommand = "<PST>EL?</PST>";

    sendCommand(azCommand);
    sendCommand(elCommand);

    timeoutTimer.start(COMMAND_TIMEOUT);
}

void PSTRotDrv::commandSleep()
{
#ifdef Q_OS_WIN
        Sleep(100);
#else
        usleep(100000);
#endif
}

void PSTRotDrv::sendCommand(const QString &cmd)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << cmd;

    QUdpSocket sendSocket;

    sendSocket.writeDatagram(cmd.toUtf8(),
                             hostInfo.addresses().at(0),
                             rotProfile.netport);
    commandSleep();
}

void PSTRotDrv::readPendingDatagrams()
{

    FCT_IDENTIFICATION;

    timeoutTimer.stop();

    while ( receiveSocket.hasPendingDatagrams() )
    {
        QNetworkDatagram datagram = receiveSocket.receiveDatagram();
        QString data(datagram.data());

        qCDebug(runtime) << "Received from" << datagram.senderAddress();
        qCDebug(runtime) << data;

        // TODO: Check if sender has the IP Address from ROT Profile?

        // TODO: we assume that the entire response fits into one packet,
        // so there is no need to implement sequential loading of fragmented packets.
        // But we prefer to check whether the length is at least expected.
        if ( data.size() < 4 )
        {
            qCWarning(runtime) << "Unexpected lenght of packet !!! - skipping";
            continue;
        }

        double newAzimuth = azimuth;
        double newElevation = elevation;

        if ( data.startsWith("EL") )
            newElevation = data.mid(3).toDouble();
        else if ( data.startsWith("AZ") )
            newAzimuth = data.mid(3).toDouble();

        qCDebug(runtime) << "PSTRotator Positioning"
                         << newAzimuth
                         << newElevation;
        qCDebug(runtime) << "Object Positioning"
                         << azimuth
                         << elevation;

        if ( newAzimuth != azimuth
             || newElevation != elevation
             || forceSendState )
        {
            forceSendState = false;
            azimuth = newAzimuth;
            elevation = newElevation;
            qCDebug(runtime) << "emitting POSITIONING changed" << azimuth << elevation;
            emit positioningChanged(azimuth, elevation);
        }
    }
}
#undef MUTEXLOCKER
#undef POOL_INTERVAL
