#include <QRegularExpression>
#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "HamlibRotDrv.h"
#include "core/debug.h"
#include "core/SerialPort.h"
#include "data/AntProfile.h"

#define MUTEXLOCKER     qCDebug(runtime) << "Waiting for Rot Drv mutex"; \
                        QMutexLocker locker(&drvLock); \
                        qCDebug(runtime) << "Using Rot Drv"

#ifndef HAMLIB_FILPATHLEN
#define HAMLIB_FILPATHLEN FILPATHLEN
#endif

#define POOL_INTERVAL 500

MODULE_IDENTIFICATION("qlog.rotator.driver.hamlibdrv");

QList<QPair<int, QString> > HamlibRotDrv::getModelList()
{
    FCT_IDENTIFICATION;

    QList<QPair<int, QString>> ret;

    rot_load_all_backends();
    rot_list_foreach(addRig, &ret);

    return ret;
}

RotCaps HamlibRotDrv::getCaps(int model)
{
    FCT_IDENTIFICATION;

    const struct rot_caps *caps = rot_get_caps(model);
    RotCaps ret;

    ret.isNetworkOnly = (model == RIG_MODEL_NETRIGCTL);

    ret.serialDataBits = caps->serial_data_bits;
    ret.serialStopBits = caps->serial_stop_bits;

    return ret;
}

int HamlibRotDrv::addRig(const rot_caps *caps, void *data)
{
    FCT_IDENTIFICATION;

    QList<QPair<int, QString>> *list = static_cast<QList<QPair<int, QString>>*>(data);

    QString name = QString("%1 %2 (%3)").arg(caps->mfg_name,
                                             caps->model_name,
                                             caps->version);

    list->append(QPair<int, QString>(caps->rot_model, name));
    return -1;
}

HamlibRotDrv::HamlibRotDrv(const RotProfile &profile,
                     QObject *parent)
    : GenericRotDrv{profile, parent},
      rot(nullptr),
      forceSendState(false)
{
    FCT_IDENTIFICATION;

    rot_load_all_backends();

    rot = rot_init(rotProfile.model);

    if ( !rot )
    {
        // initialization failed
        qCDebug(runtime) << "Cannot allocate Rotator structure";
        lastErrorText = tr("Initialization Error");
    }

    rig_set_debug(RIG_DEBUG_BUG);
}

HamlibRotDrv::~HamlibRotDrv()
{
    FCT_IDENTIFICATION;

    if ( !drvLock.tryLock(200) )
    {
        qCDebug(runtime) << "Waited too long";
        // better to make a memory leak
        return;
    }

    if ( rot )
    {
        rot_close(rot);
        rot_cleanup(rot);
        rot = nullptr;
    }
    drvLock.unlock();
}

bool HamlibRotDrv::open()
{
    FCT_IDENTIFICATION;

    MUTEXLOCKER;

    if ( !rot )
    {
        // initialization failed
        lastErrorText = tr("Initialization Error");
        qCDebug(runtime) << "Rot is not initialized";
        return false;
    }

    RotProfile::rotPortType portType = rotProfile.getPortType();

    if ( portType == RotProfile::NETWORK_ATTACHED )
    {
        //handling Network Radio
        const QString portString = rotProfile.hostname + ":" + QString::number(rotProfile.netport);
        strncpy(rot->state.rotport.pathname, portString.toLocal8Bit().constData(), HAMLIB_FILPATHLEN - 1);
    }
    else if ( portType == RotProfile::SERIAL_ATTACHED )
    {
        //handling Serial Port Radio
        strncpy(rot->state.rotport.pathname, rotProfile.portPath.toLocal8Bit().constData(), HAMLIB_FILPATHLEN - 1);
        rot->state.rotport.parm.serial.rate = rotProfile.baudrate;
        rot->state.rotport.parm.serial.data_bits = rotProfile.databits;
        rot->state.rotport.parm.serial.stop_bits = rotProfile.stopbits;
        rot->state.rotport.parm.serial.handshake = stringToHamlibFlowControl(rotProfile.flowcontrol);
        rot->state.rotport.parm.serial.parity = stringToHamlibParity(rotProfile.parity);
    }
    else
    {
        lastErrorText = tr("Unsupported Rotator Driver");
        qCDebug(runtime) << "Rot Open Error" << lastErrorText;
        return false;
    }

    int status = rot_open(rot);

    if ( status != RIG_OK )
    {
        lastErrorText = hamlibErrorString(status);
        qCDebug(runtime) << "Rot Open Error" << lastErrorText;
        return false;
    }
    else
    {
        qCDebug(runtime) << "Rot Open - OK";
    }

    opened = true;

    connect(&timer, &QTimer::timeout, this, &HamlibRotDrv::checkRotStateChange);
    timer.start(POOL_INTERVAL);
    emit rotIsReady();
    return true;
}

void HamlibRotDrv::sendState()
{
    FCT_IDENTIFICATION;

    MUTEXLOCKER;

    forceSendState = true;
}

void HamlibRotDrv::setPosition(double in_azimuth, double in_elevation)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << in_azimuth << in_elevation;

    MUTEXLOCKER;

    if ( !rot )
    {
        qCWarning(runtime) << "Rot is not active";
        return;
    }

    in_azimuth -= AntProfilesManager::instance()->getCurProfile1().azimuthOffset;

    if ( in_azimuth > 180.0 )
    {
        in_azimuth = in_azimuth - 360.0;
    }

    int status = rot_set_position(rot, static_cast<azimuth_t>(in_azimuth), static_cast<elevation_t>(in_elevation));

    if ( status != RIG_OK )
    {
        lastErrorText = hamlibErrorString(status);
        qCWarning(runtime) << "Set Az/El error" << lastErrorText;
        emit errorOccured(tr("Set Possition Error"),
                          lastErrorText);
    }

    // wait a moment because Rotators are slow and they are not possible to set and get
    // mode so quickly (get mode is called in the main thread's update() function
    commandSleep();
}

void HamlibRotDrv::stopTimers()
{
    FCT_IDENTIFICATION;

    timer.stop();
}

void HamlibRotDrv::checkRotStateChange()
{
    FCT_IDENTIFICATION;

    if ( !drvLock.tryLock(200) )
    {
        qCDebug(runtime) << "Waited too long";
        return;
    }

    qCDebug(runtime) << "Getting Rot state";

    checkChanges();

    forceSendState = false;

    // restart timer
    timer.start(POOL_INTERVAL);
    drvLock.unlock();
}

void HamlibRotDrv::checkChanges()
{
    FCT_IDENTIFICATION;

    checkAzEl();
}

void HamlibRotDrv::checkAzEl()
{
    FCT_IDENTIFICATION;

    if ( !rot )
    {
        qCWarning(runtime) << "Rot is not active";
        return;
    }

    if ( rot->caps->get_position )
    {
        azimuth_t az;
        elevation_t el;

        int status = rot_get_position(rot, &az, &el);
        if ( status == RIG_OK )
        {
            double newAzimuth = az;
            double newElevation = el;
            // Azimuth Normalization (-180,180) -> (0,360) - ADIF defined interval is 0-360
            newAzimuth += AntProfilesManager::instance()->getCurProfile1().azimuthOffset;
            newAzimuth = (newAzimuth < 0.0 ) ? 360.0 + newAzimuth : newAzimuth;

             qCDebug(runtime) << "Rot Position: " << newAzimuth << newElevation;
             qCDebug(runtime) << "Object Position: "<< azimuth << elevation;

            if ( newAzimuth != azimuth
                 || newElevation != elevation
                 || forceSendState)
            {
                azimuth = newAzimuth;
                elevation = newElevation;
                qCDebug(runtime) << "emitting POSITIONING changed";
                emit positioningChanged(azimuth, elevation);
            }
        }
        else
        {
            lastErrorText = hamlibErrorString(status);
            qCWarning(runtime) << "Get AZ/EL error" << lastErrorText;
            emit errorOccured(tr("Get Possition Error"),
                              lastErrorText);
        }
    }
    else
    {
        qCDebug(runtime) << "Get POSITION is disabled";
    }
}

serial_handshake_e HamlibRotDrv::stringToHamlibFlowControl(const QString &in_flowcontrol)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << in_flowcontrol;

    const QString &flowcontrol = in_flowcontrol.toLower();

    if ( flowcontrol == SerialPort::SERIAL_FLOWCONTROL_SOFTWARE )
        return RIG_HANDSHAKE_XONXOFF;
    if ( flowcontrol == SerialPort::SERIAL_FLOWCONTROL_HARDWARE )
        return RIG_HANDSHAKE_HARDWARE;

    return RIG_HANDSHAKE_NONE;
}

serial_parity_e HamlibRotDrv::stringToHamlibParity(const QString &in_parity)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << in_parity;

    const QString &parity = in_parity.toLower();

    if ( parity == SerialPort::SERIAL_PARITY_EVEN )
        return RIG_PARITY_EVEN;
    if ( parity == SerialPort::SERIAL_PARITY_ODD )
        return RIG_PARITY_ODD;
    if ( parity == SerialPort::SERIAL_PARITY_MARK )
        return RIG_PARITY_MARK;
    if ( parity == SerialPort::SERIAL_PARITY_SPACE )
        return RIG_PARITY_SPACE;

    return RIG_PARITY_NONE;
}

QString HamlibRotDrv::hamlibErrorString(int errorCode)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << errorCode;

    static QRegularExpression re("[\r\n]");

    const QStringList &errorList = QString(rigerror(errorCode)).split(re);
    QString ret;

    if ( errorList.size() >= 1 )
    {
        ret = errorList.at(0);
    }

    qCDebug(runtime) << ret;

    return ret;
}

void HamlibRotDrv::commandSleep()
{
#ifdef Q_OS_WIN
        Sleep(100);
#else
        usleep(100000);
#endif
}

#undef MUTEXLOCKER
#undef POOL_INTERVAL
