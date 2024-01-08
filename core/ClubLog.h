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
    static const QString getPassword();

    static void saveUsernamePassword(const QString &, const QString &);

    void uploadAdif(QByteArray &data,
                    const QString &callsignCallbook,
                    bool clearFlag = false);
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
};

#endif // CLUBLOG_H
