#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDataStream>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QDateTime>
#include <QHostAddress>

#include "Wsjtx.h"
#include "data/Data.h"
#include "debug.h"

MODULE_IDENTIFICATION("qlog.core.wsjtx");

Wsjtx::Wsjtx(QObject *parent) :
    QObject(parent),
    socket(nullptr)
{
    FCT_IDENTIFICATION;
    socket = new QUdpSocket(this);
    openPort();
    connect(socket, &QUdpSocket::readyRead, this, &Wsjtx::readPendingDatagrams);
}

void Wsjtx::openPort()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    if ( ! socket )
    {
        return;
    }

    socket->close();

    int newPort = settings.value(Wsjtx::CONFIG_PORT,Wsjtx::DEFAULT_PORT).toInt();

    qCDebug(runtime) << "New port "<< newPort;

    socket->bind(QHostAddress::Any, newPort);
}

void Wsjtx::forwardDatagram(const QNetworkDatagram &datagram)
{
    FCT_IDENTIFICATION;

    QString forwardAddresses = getConfigForwardAddresses();

    if ( forwardAddresses.isEmpty() )
    {
        qCDebug(runtime) << "Wsjtx forward list is empty";
        return;
    }

    QStringList addresses = forwardAddresses.split(" ");

    for ( const QString &address : qAsConst(addresses) )
    {
        qCDebug(runtime) << "Forwarding to address " << address;

        QStringList addressPort = address.split(":");

        if ( addressPort.size() == 2 )
        {
            bool isPortOK = false;
            uint port = addressPort.at(1).toUInt(&isPortOK);

            if ( isPortOK and port < 65536 )
            {
                QUdpSocket udpSocket(this);
                qCDebug(runtime) << "Sending to " << address;
                udpSocket.writeDatagram(datagram.data(), QHostAddress(addressPort.at(0)), port);
            }
            else
            {
                qCInfo(runtime) << "Malformed WSJTX Forward port " << addressPort.at(1);
            }
        }
        else
        {
            qCInfo(runtime) << "Malformed WSJTX Forward address " << address;
        }
    }
}

float Wsjtx::modePeriodLenght(const QString &mode)
{
    FCT_IDENTIFICATION;

    float ret = 60;

    qCDebug(function_parameters) << mode;

    if ( mode == "FST4"
         || mode == "FT8"
         || mode == "MSK144" )
    {
        ret = 15;
    }
    else if ( mode == "FT4" )
    {
        ret = 7.5;
    }
    else if ( mode == "JT4"
              || mode == "JT9"
              || mode.contains("JT65")
              || mode == "QRA64"
              || mode == "ISCAT" )
    {
        ret = 60;
    }
    else if ( mode == "FST4W"
              || mode == "WSPR" )
    {
        ret = 120;
    }

    qCDebug(runtime) << "Period: " << ret;

    return ret;
}

quint16 Wsjtx::getConfigPort()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(Wsjtx::CONFIG_PORT, Wsjtx::DEFAULT_PORT).toInt();
}

void Wsjtx::saveConfigPort(quint16 port)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue(Wsjtx::CONFIG_PORT, port);
}

QString Wsjtx::getConfigForwardAddresses()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(Wsjtx::CONFIG_FORWARD_ADDRESSES).toString();
}

void Wsjtx::saveConfigForwardAddresses(const QString &addresses)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue(Wsjtx::CONFIG_FORWARD_ADDRESSES, addresses);
}

void Wsjtx::readPendingDatagrams()
{
    FCT_IDENTIFICATION;

    while (socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();

        wsjtxAddress = datagram.senderAddress();
        wsjtxPort = datagram.senderPort();

        QDataStream stream(datagram.data());

        quint32 magic, schema, mtype;
        stream >> magic;
        stream >> schema;
        stream >> mtype;

        if (magic != 0xadbccbda) {
            qCDebug(runtime) << "Invalid wsjtx magic";
            continue;
        }

        qCDebug(runtime) << "WSJT mtype << "<< mtype << " schema " << schema;

        forwardDatagram(datagram);

        switch (mtype) {
        /* WSJTX Status message */
        case 1: {
            QByteArray id, mode, tx_mode, sub_mode, report, dx_call, dx_grid, de_call, de_grid, conf_name, tx_message;
            WsjtxStatus status;


            stream >> id >> status.dial_freq >> mode >> dx_call >> report >> tx_mode;
            stream >> status.tx_enabled >> status.transmitting >> status.decoding;
            stream >> status.rx_df >> status.tx_df >> de_call >> de_grid >> dx_grid;
            stream >> status.tx_watchdog >> sub_mode >> status.fast_mode >> status.special_op_mode >> status.freq_tolerance;
            stream >> status.tr_period >> conf_name >> tx_message;

            status.id = QString(id);
            status.mode = QString(mode);
            status.tx_mode = QString(mode);
            status.sub_mode = QString(sub_mode);
            status.report = QString(report);
            status.dx_call = QString(dx_call);
            status.de_call = QString(de_call);
            status.de_grid = QString(de_grid);
            status.conf_name = QString(conf_name);
            status.tx_message = QString(tx_message);

            qCDebug(runtime)<<status;

            emit statusReceived(status);
            break;
        }
        /* WSJTX Decode message */
        case 2: {
            QByteArray id, mode, message;
            WsjtxDecode decode;

            stream >> id >> decode.is_new >> decode.time >> decode.snr >> decode.dt >> decode.df;
            stream >> mode >> message >> decode.low_confidence >> decode.off_air;

            decode.id = QString(id);
            decode.mode = QString(mode);
            decode.message = QString(message);

            qCDebug(runtime) << decode;

            emit decodeReceived(decode);
            break;
        }
        /* WSJTX Log message */
        case 5: {
            QByteArray id, dx_call, dx_grid, mode, rprt_sent, rprt_rcvd, tx_pwr, comments;
            QByteArray name, op_call, my_call, my_grid, exch_sent, exch_rcvd, prop_mode;
            WsjtxLog log;

            stream >> id >> log.time_off >> dx_call >> dx_grid >> log.tx_freq >> mode >> rprt_sent;
            stream >> rprt_rcvd >> tx_pwr >> comments >> name >> log.time_on >> op_call;
            stream >> my_call >> my_grid >> exch_sent >> exch_rcvd >> prop_mode;

            log.id = QString(id);
            log.dx_call = QString(dx_call).toUpper();
            log.dx_grid = QString(dx_grid).toUpper();
            log.mode = QString(mode);
            log.rprt_sent = QString(rprt_sent);
            log.rprt_rcvd = QString(rprt_rcvd);
            log.tx_pwr = QString(tx_pwr);
            log.comments = QString(comments);
            log.name = QString(name);
            log.op_call = QString(op_call).toUpper();
            log.my_call = QString(my_call).toUpper();
            log.my_grid = QString(my_grid).toUpper();
            log.exch_sent = QString(exch_sent);
            log.exch_rcvd = QString(exch_rcvd);
            log.prop_mode = QString(prop_mode);

            qCDebug(runtime) << log;

            insertContact(log);
            break;
        }
        /* WSJTX LogADIF message */
        case 12: {
            QByteArray id, adif_text;
            WsjtxLogADIF log;

            stream >> id >> adif_text;

            log.id = QString(id);
            log.log_adif = QString(adif_text);

            qCDebug(runtime) << log;
            break;
        }
        }
    }
}

void Wsjtx::insertContact(WsjtxLog log)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << log;

    QSqlTableModel model;
    model.setTable("contacts");
    model.removeColumn(model.fieldIndex("id"));

    QSqlRecord record = model.record();

    double freq = static_cast<double>(log.tx_freq)/1e6;
    QString band = Data::band(freq).name;

    record.setValue("freq", freq);
    record.setValue("band", band);

    /* if field is empty then do not initialize it, leave it NULL
     * for database */
    if ( !log.dx_call.isEmpty() )
    {
        record.setValue("callsign", log.dx_call);
    }

    if ( !log.rprt_rcvd.isEmpty() )
    {
        record.setValue("rst_rcvd", log.rprt_rcvd);
    }

    if ( !log.rprt_sent.isEmpty() )
    {
        record.setValue("rst_sent", log.rprt_sent);
    }

    if ( !log.name.isEmpty() )
    {
        record.setValue("name", Data::removeAccents(log.name));
        record.setValue("name_intl", log.name);
    }

    if ( !log.dx_grid.isEmpty() )
    {
        record.setValue("gridsquare", log.dx_grid);
    }

    if ( !log.mode.isEmpty() )
    {
        record.setValue("mode", log.mode);
    }

    if ( log.time_on.isValid() )
    {
        record.setValue("start_time", log.time_on);
    }

    if ( log.time_off.isValid() )
    {
        record.setValue("end_time", log.time_off);
    }

    if ( !log.comments.isEmpty() )
    {
        record.setValue("comment", Data::removeAccents(log.comments));
        record.setValue("comment_intl", log.comments);
    }

    if ( !log.exch_sent.isEmpty() )
    {
        record.setValue("stx_string", log.exch_sent);
    }

    if ( !log.exch_rcvd.isEmpty() )
    {
        record.setValue("srx_string", log.exch_rcvd);
    }

    if ( !log.prop_mode.isEmpty() )
    {
        record.setValue("prop_mode", log.prop_mode);
    }

    if ( !log.tx_pwr.isEmpty() )
    {
        record.setValue("tx_pwr", log.tx_pwr);
    }

    if ( !log.op_call.isEmpty() )
    {
        record.setValue("operator", Data::removeAccents(log.op_call));
    }

    if ( !log.my_grid.isEmpty() )
    {
        record.setValue("my_gridsquare", log.my_grid);
    }

    if ( !log.my_call.isEmpty() )
    {
        record.setValue("station_callsign", log.my_call);
    }

    emit addContact(record);
}

void Wsjtx::startReply(WsjtxDecode decode)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << decode;

    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream << static_cast<quint32>(0xadbccbda);
    stream << static_cast<quint32>(3);
    stream << static_cast<quint32>(4);
    stream << decode.id.toUtf8();
    stream << decode.time;
    stream << decode.snr;
    stream << decode.dt;
    stream << decode.df;
    stream << decode.mode.toUtf8();
    stream << decode.message.toUtf8();
    stream << decode.low_confidence;
    stream << static_cast<quint8>(0);

    socket->writeDatagram(data, wsjtxAddress, wsjtxPort);
}

void Wsjtx::reloadSetting()
{
    FCT_IDENTIFICATION;
    openPort();
}

QString Wsjtx::CONFIG_PORT = "network/wsjtx_port";
int     Wsjtx::DEFAULT_PORT = 2237;
QString Wsjtx::CONFIG_FORWARD_ADDRESSES = "networt/wsjtx_forward";
