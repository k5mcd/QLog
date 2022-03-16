#ifndef HOSTSPORTSTRING_H
#define HOSTSPORTSTRING_H

#include <QObject>
#include <QHostAddress>

class HostPortAddress : public QHostAddress
{
public:
    explicit HostPortAddress( const QString &, quint16);
    void setPort(quint16);
    quint16 getPort() const;

private:
    quint16 port;
};

class HostsPortString : public QObject
{
    Q_OBJECT
public:
    explicit HostsPortString(const QString &, QObject *parent=nullptr);

    QList<HostPortAddress> getAddrList() const;

private:

    QList<HostPortAddress> addressList;

};

#endif // HOSTSPORTSTRING_H
