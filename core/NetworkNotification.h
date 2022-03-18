#ifndef NETWORKNOTIFICATION_H
#define NETWORKNOTIFICATION_H

#include <QObject>
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include "core/HostsPortString.h"
#include "logformat/LogFormat.h"
#include "data/DxSpot.h"

class GenericNotificationMsg : public QObject
{
    Q_OBJECT

public:
    explicit GenericNotificationMsg(QObject *parent = nullptr);
    QByteArray getJson() const  { return QJsonDocument(msg).toJson(QJsonDocument::Compact); };

protected:
    QJsonObject msg;
};

class QSONotificationMsg : public GenericNotificationMsg
{

public:
    enum QSOOperation
    {
        QSO_INSERT = 0,
        QSO_UPDATE = 1,
        QSO_DELETE = 2
    };

    explicit QSONotificationMsg(const QSqlRecord&,
                                const QSOOperation,
                                QObject *parent = nullptr);

private:
    QMap<int, QString> QSOOperation2String = {
        {QSOOperation::QSO_INSERT, "insert"},
        {QSOOperation::QSO_UPDATE, "update"},
        {QSOOperation::QSO_DELETE, "delete"},
    };
};

class DXSpotNotificationMsg : public GenericNotificationMsg
{
public:
    explicit DXSpotNotificationMsg(const DxSpot&, QObject *parent = nullptr);

private:
    QMap<int, QString> DxccStatus2String = {
        {DxccStatus::NewEntity, "newentity"},
        {DxccStatus::NewBandMode, "newbandmode"},
        {DxccStatus::NewBand, "newband"},
        {DxccStatus::NewMode, "newmode"},
        {DxccStatus::NewSlot, "newslot"},
        {DxccStatus::Worked, "worked"},
        {DxccStatus::UnknownStatus, "unknown"},
    };
};


class NetworkNotification : public QObject
{
    Q_OBJECT
public:
    explicit NetworkNotification(QObject *parent = nullptr);

    static QString getNotifQSOAdiAddrs();
    static void saveNotifQSOAdiAddrs(const QString &);
    static QString getNotifDXSpotAddrs();
    static void saveNotifDXSpotAddrs(const QString &);


public slots:
    void QSOInserted(const QSqlRecord &);
    void QSOUpdated(const QSqlRecord &);
    void QSODeleted(const QSqlRecord &);
    void dxSpot(const DxSpot&);

private:

    void send(const QByteArray &, const HostsPortString &);

    static QString CONFIG_NOTIF_QSO_ADI_ADDRS_KEY;
    static QString CONFIG_NOTIF_DXSPOT_ADDRS_KEY;

};

#endif // NETWORKNOTIFICATION_H
