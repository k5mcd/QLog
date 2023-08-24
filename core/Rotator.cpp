#include <hamlib/rotator.h>
#include "Rotator.h"
#include "core/debug.h"
#include "data/RotProfile.h"
#include "data/AntProfile.h"

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <unistd.h>
#endif

MODULE_IDENTIFICATION("qlog.core.rotator");

#ifndef HAMLIB_FILPATHLEN
#define HAMLIB_FILPATHLEN FILPATHLEN
#endif

// at this moment it is not needed to define customized poll interval.
// poll interval is build-in 500ms
#define STARTING_UPDATE_INTERVAL 500
#define SLOW_UPDATE_INTERVAL 2000

Rotator::Rotator(QObject *parent) :
    SerialPort(parent),
    timer(nullptr),
    forceSendState(false)
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

    QMutexLocker locker(&rotLock);
    return azimuth;
}

int Rotator::getElevation()
{
    FCT_IDENTIFICATION;

    QMutexLocker locker(&rotLock);
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

void Rotator::sendState()
{
    FCT_IDENTIFICATION;

    if ( ! rot )
        return;

    QMutexLocker locker(&rotLock);
    forceSendState = true;
}

void Rotator::start() {
    FCT_IDENTIFICATION;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Rotator::update);
    timer->start(STARTING_UPDATE_INTERVAL);
}

void Rotator::update()
{
    FCT_IDENTIFICATION;

    if ( !isRotConnected() )
    {
        forceSendState = false;
        /* rot is not connected, slow down */
        timer->start(SLOW_UPDATE_INTERVAL);
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
        timer->start(STARTING_UPDATE_INTERVAL); // fix time is correct, it is not necessary to change.
        forceSendState = false;
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
        int status = rot_get_position(rot, &az, &el);
        if ( status == RIG_OK )
        {
            int newAzimuth = static_cast<int>(az);
            int newElevation = static_cast<int>(el);
            // Azimuth Normalization (-180,180) -> (0,360) - ADIF defined interval is 0-360
            newAzimuth += AntProfilesManager::instance()->getCurProfile1().azimuthOffset;
            newAzimuth = (newAzimuth < 0 ) ? 360 + newAzimuth : newAzimuth;

            if ( newAzimuth != this->azimuth
                 || newElevation != this->elevation
                 || forceSendState)
            {
                this->azimuth = newAzimuth;
                this->elevation = newElevation;
                emit positionChanged(azimuth, elevation);
            }
        }
        else
        {
            __closeRot();
            emit rotErrorPresent(tr("Get Position Error"),
                                 hamlibErrorString(status));
        }
    }
    else
    {
        qCDebug(runtime) << "Get POSITION is disabled";
    }

    timer->start(STARTING_UPDATE_INTERVAL);
    forceSendState = false;
    rotLock.unlock();
}
void Rotator::open()
{
    FCT_IDENTIFICATION;

    QMetaObject::invokeMethod(this, &Rotator::openImpl, Qt::QueuedConnection);
}

void Rotator::openImpl()
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

    if ( newRotProfile == RotProfile() )
    {
        emit rotErrorPresent(tr("No Rotator Profile selected"),
                             QString());
        return;
    }

    qCDebug(runtime) << "Opening profile name: " << newRotProfile.profileName;

    rot = rot_init(newRotProfile.model);

    if ( !isRotConnected() )
    {
        // initialization failed
        emit rotErrorPresent(tr("Initialization Error"),
                             QString());
        return;
    }

    if ( newRotProfile.getPortType() == RotProfile::NETWORK_ATTACHED )
    {
        // handling network rotator
        QString portString = newRotProfile.hostname + ":" + QString::number(newRotProfile.netport);
        strncpy(rot->state.rotport.pathname, portString.toLocal8Bit().constData(), HAMLIB_FILPATHLEN - 1);
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
        emit rotErrorPresent(tr("Open Connection Error"),
                             hamlibErrorString(status));
        return;
    }

    emit rotConnected();

    connectedRotProfile = newRotProfile;
}

QString Rotator::hamlibErrorString(int errorCode)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << errorCode;
    static QRegularExpression re("[\r\n]");

    QStringList errorList = QString(rigerror(errorCode)).split(re);
    QString ret;

    if ( errorList.size() >= 1 )
    {
        ret = errorList.at(0);
    }

    qCDebug(runtime) << ret;

    return ret;
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

    QMetaObject::invokeMethod(this, &Rotator::closeImpl, Qt::QueuedConnection);
}

void Rotator::closeImpl()
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

    azimuth -= AntProfilesManager::instance()->getCurProfile1().azimuthOffset;

    rotLock.lock();

    if ( azimuth > 180 )
    {
        azimuth = azimuth - 360;
    }

    int status = rot_set_position(rot, static_cast<azimuth_t>(azimuth), static_cast<elevation_t>(elevation));

    if (status != RIG_OK)
    {
        __closeRot();
        emit rotErrorPresent(tr("Set Possition Error"),
                             hamlibErrorString(status));
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
