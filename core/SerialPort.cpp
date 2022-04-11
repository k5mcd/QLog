#include "SerialPort.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.core.serialport");

SerialPort::SerialPort(QObject *parent)
    : QObject{parent}
{
    FCT_IDENTIFICATION;

}

serial_handshake_e SerialPort::stringToFlowControl(const QString &in_flowcontrol)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << in_flowcontrol;

    QString flowcontrol = in_flowcontrol.toLower();

    if ( flowcontrol == SerialPort::SERIAL_FLOWCONTROL_SOFTWARE )
        return RIG_HANDSHAKE_XONXOFF;
    if ( flowcontrol == SerialPort::SERIAL_FLOWCONTROL_HARDWARE )
        return RIG_HANDSHAKE_HARDWARE;

    return RIG_HANDSHAKE_NONE;

}

serial_parity_e SerialPort::stringToParity(const QString &in_parity)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << in_parity;

    QString parity = in_parity.toLower();

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

const QString SerialPort::SERIAL_FLOWCONTROL_NONE = "none";
const QString SerialPort::SERIAL_FLOWCONTROL_HARDWARE = "hardware";
const QString SerialPort::SERIAL_FLOWCONTROL_SOFTWARE = "software";
const QString SerialPort::SERIAL_PARITY_EVEN = "even";
const QString SerialPort::SERIAL_PARITY_ODD = "odd";
const QString SerialPort::SERIAL_PARITY_MARK = "mark";
const QString SerialPort::SERIAL_PARITY_SPACE  = "space";
const QString SerialPort::SERIAL_PARITY_NO  = "no";
