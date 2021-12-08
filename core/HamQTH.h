#ifndef HAMQTH_H
#define HAMQTH_H

#include <QObject>
#include <QString>
#include "core/GenericCallbook.h"

class QNetworkAccessManager;
class QNetworkReply;

class HamQTH : public GenericCallbook
{
    Q_OBJECT

public:
    explicit HamQTH(QObject *parent = nullptr);
    ~HamQTH();

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

#endif // HAMQTH_H
