#ifndef NETWORKNOTIFICATION_H
#define NETWORKNOTIFICATION_H

#include <QObject>
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include "core/HostsPortString.h"
#include "logformat/LogFormat.h"

class GenericNotificationMsg : public QObject
{
    Q_OBJECT

public:
    explicit GenericNotificationMsg(QObject *parent = nullptr);
    QByteArray getJson() const  { return QJsonDocument(msg).toJson(); };

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


class NetworkNotification : public QObject
{
    Q_OBJECT
public:
    explicit NetworkNotification(QObject *parent = nullptr);

    static QString getNotifQSOAdiAddrs();
    static void saveNotifQSOAdiAddrs(const QString &);

public slots:
    void QSOInserted(const QSqlRecord &);
    void QSOUpdated(const QSqlRecord &);
    void QSODeleted(const QSqlRecord &);

private:

    void send(const QByteArray &, const HostsPortString &);

    static QString CONFIG_NOTIF_QSO_ADI_ADDRS_KEY;

};

#endif // NETWORKNOTIFICATION_H
