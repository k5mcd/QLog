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
    const static QString SECURE_STORAGE_KEY;
    const static QString CONFIG_EMAIL_KEY;
    const static QString CONFIG_CALLSIGN_KEY;

signals:
    void uploadOK(QString);
    void uploadError(QString);

public slots:
    void uploadContact(QSqlRecord record);
    QNetworkReply* uploadAdif(QByteArray &data);
    void processReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* nam;
};

#endif // CLUBLOG_H
