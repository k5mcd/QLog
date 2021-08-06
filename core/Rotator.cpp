#include <hamlib/rotator.h>
#include "Rotator.h"

#ifndef HAMLIB_FILPATHLEN
#define HAMLIB_FILPATHLEN FILPATHLEN
#endif

static enum serial_handshake_e stringToFlowControl(const QString in_flowcontrol)
{
    QString flowcontrol = in_flowcontrol.toLower();

    if ( flowcontrol == "software" ) return RIG_HANDSHAKE_XONXOFF;
    if ( flowcontrol == "hardware" ) return RIG_HANDSHAKE_HARDWARE;
    return RIG_HANDSHAKE_NONE;
}

static enum serial_parity_e stringToParity(const QString in_parity)
{
    QString parity = in_parity.toLower();

    if ( parity == "even" ) return RIG_PARITY_EVEN;
    if ( parity == "odd" ) return RIG_PARITY_ODD;
    if ( parity == "mark" ) return RIG_PARITY_MARK;
    if ( parity == "space" ) return RIG_PARITY_SPACE;

    return RIG_PARITY_NONE;
}

Rotator::Rotator() : QObject(nullptr)
{

}

Rotator* Rotator::instance() {
    static Rotator instance;
    return &instance;
}

void Rotator::start() {
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

void Rotator::update() {
    if (!rot) return;

    if (!rotLock.tryLock(200)) return;

    azimuth_t az;
    elevation_t el;

    rot_get_position(rot, &az, &el);

    int newAzimuth = static_cast<int>(az);
    int newElevation = static_cast<int>(el);

    if (newAzimuth != this->azimuth || newElevation != this->elevation)  {
        this->azimuth = newAzimuth;
        this->elevation = newElevation;
        emit positionChanged(azimuth, elevation);
    }
    rotLock.unlock();
}

void Rotator::open() {
    QSettings settings;
    int model = settings.value("hamlib/rot/model").toInt();
    int baudrate = settings.value("hamlib/rot/baudrate").toInt();
    int databits = settings.value("hamlib/rot/databits").toInt();
    float stopbits = settings.value("hamlib/rot/stopbits").toFloat();
    QString flowControl = settings.value("hamlib/rot/stopbits").toString();
    QString parity = settings.value("hamlib/rot/parity").toString();
    QByteArray portStr = settings.value("hamlib/rot/port").toByteArray();
    QString hostname = settings.value("hamlib/rot/hostname").toString();
    int netport = settings.value("hamlib/rot/netport").toInt();

    const char* port = portStr.constData();

    qDebug() << portStr;

    rotLock.lock();

    // if rot is active then close it
    //close(); // do not call close here because rot is already locked by mutex
    if (rot)
    {
        rot_close(rot);
        rot_cleanup(rot);
    }

    rot = rot_init(model);

    if ( rot->caps->port_type == RIG_PORT_NETWORK
         || rot->caps->port_type == RIG_PORT_UDP_NETWORK )
    {
        // handling network rotator
        strncpy(rot->state.rotport.pathname, hostname.toLocal8Bit().constData(), HAMLIB_FILPATHLEN - 1);
        //port is hardcoded in hamlib - not necessary to set it.
        (void)netport;
    }
    else
    {
        // handling serial rotator
        strncpy(rot->state.rotport.pathname, port, HAMLIB_FILPATHLEN - 1);
        rot->state.rotport.parm.serial.rate = baudrate;
        rot->state.rotport.parm.serial.data_bits = databits;
        rot->state.rotport.parm.serial.stop_bits = stopbits;
        rot->state.rotport.parm.serial.handshake = stringToFlowControl(flowControl);
        rot->state.rotport.parm.serial.parity = stringToParity(parity);
    }

    int status = rot_open(rot);

    rotLock.unlock();

    if (status != RIG_OK) {
        qWarning() << "rotator connection error";
    }
    else {
        qDebug() << "connected to rotator";
    }
}

void Rotator::close()
{
    rotLock.lock();
    if (rot)
    {
        rot_close(rot);
        rot_cleanup(rot);
        rot = nullptr;
    }
    rotLock.unlock();
}

void Rotator::setPosition(int azimuth, int elevation) {
    if (!rot) return;
    rotLock.lock();
    rot_set_position(rot, static_cast<azimuth_t>(azimuth), static_cast<elevation_t>(elevation));
    rotLock.unlock();
}
