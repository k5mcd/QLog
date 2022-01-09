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

    static const QString getEmail();
    static const QString getRegisteredCallsign();
    static const QString getPassword();

    static void saveRegistredCallsign(const QString &);
    static void saveUsernamePassword(const QString &, const QString &);

signals:
    void uploadOK(QString);
    void uploadError(QString);

public slots:
    void uploadContact(QSqlRecord record);
    QNetworkReply* uploadAdif(QByteArray &data);
    void processReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* nam;

    const static QString SECURE_STORAGE_KEY;
    const static QString CONFIG_EMAIL_KEY;
    const static QString CONFIG_CALLSIGN_KEY;
};

#endif // CLUBLOG_H
