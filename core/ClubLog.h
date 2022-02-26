#ifndef CLUBLOG_H
#define CLUBLOG_H

#include <QObject>
#include <QSqlRecord>

class QNetworkReply;
class QNetworkAccessManager;

class ClubLog : public QObject
{
    Q_OBJECT
public:
    explicit ClubLog(QObject *parent = nullptr);
    ~ClubLog();

    static const QString getEmail();
    static const QString getRegisteredCallsign();
    static const QString getPassword();

    static void saveRegistredCallsign(const QString &);
    static void saveUsernamePassword(const QString &, const QString &);

    void uploadAdif(QByteArray &data);
    void uploadContact(QSqlRecord record);

signals:
    void uploadOK(QString);
    void uploadError(QString);

public slots:
    void processReply(QNetworkReply* reply);
    void abortRequest();

private:
    QNetworkAccessManager* nam;
    QNetworkReply *currentReply;

    const static QString SECURE_STORAGE_KEY;
    const static QString CONFIG_EMAIL_KEY;
    const static QString CONFIG_CALLSIGN_KEY;
};

#endif // CLUBLOG_H
