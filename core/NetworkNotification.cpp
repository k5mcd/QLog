#include <QUuid>
#include <QUdpSocket>

#include "NetworkNotification.h"
#include "debug.h"
#include "LogParam.h"

MODULE_IDENTIFICATION("qlog.ui.networknotification");

NetworkNotification::NetworkNotification(QObject *parent)
    : QObject(parent)
{
    FCT_IDENTIFICATION;
}

QString NetworkNotification::getNotifQSOAdiAddrs()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(NetworkNotification::CONFIG_NOTIF_QSO_ADI_ADDRS_KEY).toString();
}

void NetworkNotification::saveNotifQSOAdiAddrs(const QString &addresses)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue(NetworkNotification::CONFIG_NOTIF_QSO_ADI_ADDRS_KEY, addresses);
}

void NetworkNotification::QSOInserted(const QSqlRecord &record)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << "Inserted: " << record;

    HostsPortString destList(getNotifQSOAdiAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        QSONotificationMsg qsoMsg(record, QSONotificationMsg::QSO_INSERT);
        send(qsoMsg.getJson(), destList);
    }
}

void NetworkNotification::QSOUpdated(const QSqlRecord &record)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << "Updated: " << record;

    HostsPortString destList(getNotifQSOAdiAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        QSONotificationMsg qsoMsg(record, QSONotificationMsg::QSO_UPDATE);
        send(qsoMsg.getJson(), destList);
    }
}

void NetworkNotification::QSODeleted(const QSqlRecord &record)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << "Deleted: " << record;

    HostsPortString destList(getNotifQSOAdiAddrs());

    if ( destList.getAddrList().size() > 0 )
    {
        QSONotificationMsg qsoMsg(record, QSONotificationMsg::QSO_DELETE);
        send(qsoMsg.getJson(), destList);
    }
}

void NetworkNotification::send(const QByteArray &data, const HostsPortString &dests)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << QString(data);

    if ( data.size() <= 0 )
    {
        return;
    }

    QList<HostPortAddress> addrList = dests.getAddrList();

    for ( const HostPortAddress &addr : qAsConst(addrList) )
    {
        QUdpSocket udpSocket;

        qCDebug(runtime) << "Sending to " << addr;
        udpSocket.writeDatagram(data, addr, addr.getPort());
    }
}

QString NetworkNotification::CONFIG_NOTIF_QSO_ADI_ADDRS_KEY = "network/notification/qso/adi_addrs";

GenericNotificationMsg::GenericNotificationMsg(QObject *parent) :
    QObject(parent)
{
    FCT_IDENTIFICATION;

    msg["appid"] = "QLog";
    msg["time"] = QDateTime::currentMSecsSinceEpoch();
}

QSONotificationMsg::QSONotificationMsg(const QSqlRecord &record,
                                       const QSOOperation operation,
                                       QObject *parent) :
    GenericNotificationMsg(parent)
{
    FCT_IDENTIFICATION;

    QString data;
    QTextStream stream(&data, QIODevice::ReadWrite);

    /* currently only ADIF is supported */
    /* ADX and JSON do not support export single record */
    LogFormat* format = LogFormat::open(LogFormat::ADI, stream);

    if (!format) {
        qWarning() << "cannot created ADIF Export Formatter";
        return;
    }

    //format->exportStart();
    format->exportContact(record);
    stream.flush();
    //format->exportEnd();

    QJsonObject qsoData;
    qsoData["operation"] = QSOOperation2String.value(operation,"unknown");
    qsoData["type"] = "adif";
    qsoData["logid"] = LogParam::getParam("logid");
    qsoData["rowid"] = record.value(record.indexOf("id")).toInt();
    qsoData["value"] = data.replace("\n", "");

    msg["msgtype"] = "qso";
    msg["data"] = qsoData;

    delete format;
}
