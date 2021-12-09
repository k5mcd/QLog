#ifndef QRZ_H
#define QRZ_H

#include <QObject>
#include <QString>
#include "core/GenericCallbook.h"

class QNetworkAccessManager;
class QNetworkReply;

class QRZ : public GenericCallbook
{
    Q_OBJECT

public:
    explicit QRZ(QObject *parent = nullptr);
    ~QRZ();

public slots:
    void queryCallsign(QString callsign);

public slots:
    void processReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* nam;
    QString sessionId;
    QString queuedCallsign;
    bool incorrectLogin;
    QString lastSeenPassword;

    void authenticate();
};

#endif // QRZ_H
