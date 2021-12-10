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
    const static QString SECURE_STORAGE_KEY;
    const static QString CONFIG_USERNAME_KEY;
    const static QString CALLBOOK_NAME;

public slots:
    void queryCallsign(QString callsign) override;

private slots:
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
