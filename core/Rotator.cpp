#include <hamlib/rotator.h>
#include "Rotator.h"
#include "core/debug.h"
#include "data/RotProfile.h"

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <unistd.h>
#endif

MODULE_IDENTIFICATION("qlog.core.rotator");

#ifndef HAMLIB_FILPATHLEN
#define HAMLIB_FILPATHLEN FILPATHLEN
#endif

Rotator::Rotator(QObject *parent) :
    SerialPort(parent),
    timer(nullptr)
{
    FCT_IDENTIFICATION;

    azimuth = 0;
    elevation = 0;
    rot = nullptr;
    rig_set_debug(RIG_DEBUG_ERR);
}

Rotator::~Rotator()
{
    FCT_IDENTIFICATION;
}

Rotator* Rotator::instance() {
    FCT_IDENTIFICATION;

    static Rotator instance;
    return &instance;
}

int Rotator::getAzimuth()
{
    FCT_IDENTIFICATION;

    return azimuth;
}

int Rotator::getElevation()
{
    FCT_IDENTIFICATION;

    return elevation;
}

bool Rotator::isRotConnected()
{
    FCT_IDENTIFICATION;

    return (rot) ? true : false;
}

void Rotator::stopTimer()
{
    FCT_IDENTIFICATION;
    bool check = QMetaObject::invokeMethod(Rotator::instance(), &Rotator::stopTimerImplt, Qt::QueuedConnection);
    Q_ASSERT( check );
}

bool Rotator::isNetworkRot(const rot_caps *caps)
{
    FCT_IDENTIFICATION;

    bool ret = false;

    if ( caps )
    {
        ret = caps->port_type == RIG_PORT_NETWORK
               || caps->port_type == RIG_PORT_UDP_NETWORK;
    }

    qCDebug(runtime) << ret;

    return ret;

}

void Rotator::start() {
    FCT_IDENTIFICATION;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

void Rotator::update()
{
    FCT_IDENTIFICATION;

    int status = RIG_OK;

    if ( !isRotConnected() )
    {
        /* rot is not connected, slow down */
        timer->start(5000);
        return;
    }

    if (!rotLock.tryLock(200)) return;

    RotProfile currRotProfile = RotProfilesManager::instance()->getCurProfile1();

    /***********************************************************/
    /* Is Opened Profile still the globbaly used Rot Profile ? */
    /* if NO then reconnect it                                 */
    /***********************************************************/
    if ( currRotProfile != connectedRotProfile)
    {
        /* Rot Profile Changed
         * Need to reconnect rig
         */
        qCDebug(runtime) << "Reconnecting to a new RIG - " << currRotProfile.profileName << "; Old - " << connectedRotProfile.profileName;
        __openRot();
        timer->start(1000); // fix time is correct, it is not necessary to change.
        rotLock.unlock();
        return;
    }

    /*************/
    /* Get AZ/EL */
    /*************/
    azimuth_t az;
    elevation_t el;

    if ( rot->caps->get_position )
    {
        status = rot_get_position(rot, &az, &el);
        if ( status == RIG_OK )
        {
            int newAzimuth = static_cast<int>(az);
            int newElevation = static_cast<int>(el);

            if (newAzimuth != this->azimuth || newElevation != this->elevation)  {
                this->azimuth = newAzimuth;
                this->elevation = newElevation;
                emit positionChanged(azimuth, elevation);
            }
        }
        else
        {
            __closeRot();
            emit rotErrorPresent(QString(tr("Get Position Error - ")) + QString(rigerror(status)));
        }
    }
    else
    {
        qCDebug(runtime) << "Get POSITION is disabled";
    }

    timer->start(1000);
    rotLock.unlock();
}
void Rotator::open()
{
    FCT_IDENTIFICATION;

    rotLock.lock();
    __openRot();
    rotLock.unlock();
}

void Rotator::__openRot()
{
    FCT_IDENTIFICATION;

    __closeRot();

    RotProfile newRotProfile = RotProfilesManager::instance()->getCurProfile1();

    qCDebug(runtime) << "Opening profile name: " << newRotProfile.profileName;

    rot = rot_init(newRotProfile.model);

    if ( !isRotConnected() )
    {
        // initialization failed
        emit rotErrorPresent(QString(tr("Initialization Error")));
        return;
    }

    if ( isNetworkRot(rot->caps) )
    {
        // handling network rotator
        strncpy(rot->state.rotport.pathname, newRotProfile.hostname.toLocal8Bit().constData(), HAMLIB_FILPATHLEN - 1);
        //port is hardcoded in hamlib - not necessary to set it.
    }
    else
    {
        // handling serial rotator
        strncpy(rot->state.rotport.pathname, newRotProfile.portPath.toLocal8Bit().constData(), HAMLIB_FILPATHLEN - 1);
        rot->state.rotport.parm.serial.rate = newRotProfile.baudrate;
        rot->state.rotport.parm.serial.data_bits = newRotProfile.databits;
        rot->state.rotport.parm.serial.stop_bits = newRotProfile.stopbits;
        rot->state.rotport.parm.serial.handshake = stringToFlowControl(newRotProfile.flowcontrol);
        rot->state.rotport.parm.serial.parity = stringToParity(newRotProfile.parity);
    }

    int status = rot_open(rot);

    if (status != RIG_OK)
    {
        __closeRot();
        emit rotErrorPresent(QString(tr("Open Connection Error - ")) + QString(rigerror(status)));
        return;
    }

    emit rotConnected();

    connectedRotProfile = newRotProfile;
}

void Rotator::__closeRot()
{
    FCT_IDENTIFICATION;

    connectedRotProfile = RotProfile();
    azimuth = 0;
    elevation = 0;

    if (isRotConnected())
    {
        rot_close(rot);
        rot_cleanup(rot);
        rot = nullptr;
    }

    emit rotDisconnected();
}

void Rotator::close()
{
    FCT_IDENTIFICATION;

    rotLock.lock();
    __closeRot();
    rotLock.unlock();
}

void Rotator::setPositionImpl(int azimuth, int elevation) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<azimuth<< " " << elevation;

    if ( !isRotConnected() ) return;

    rotLock.lock();

    if ( azimuth > 180 )
    {
        azimuth = azimuth - 360;
    }

    int status = rot_set_position(rot, static_cast<azimuth_t>(azimuth), static_cast<elevation_t>(elevation));

    if (status != RIG_OK)
    {
        __closeRot();
        emit rotErrorPresent(QString(tr("Set Possition Error - ")) + QString(rigerror(status)));
    }

    // wait a moment because Rigs are slow and they are not possible to set and get
    // mode so quickly (get mode is called in the main thread's update() function
#ifdef Q_OS_WIN
    Sleep(100);
#else
    usleep(100000);
#endif

    rotLock.unlock();
}

void Rotator::stopTimerImplt()
{
    FCT_IDENTIFICATION;

    if ( timer )
    {
        timer->stop();
        timer->deleteLater();
        timer = nullptr;
    }
}

void Rotator::setPosition(int azimuth, int elevation)
{
    FCT_IDENTIFICATION;

    QMetaObject::invokeMethod(this, "setPositionImpl", Qt::QueuedConnection,
                              Q_ARG(int,azimuth), Q_ARG(int,elevation));
}
