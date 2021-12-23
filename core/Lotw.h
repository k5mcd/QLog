#ifndef LOTW_H
#define LOTW_H

#include <QObject>
#include <QNetworkReply>
#include <logformat/LogFormat.h>

class QNetworkAccessManager;
class QNetworkReply;

class Lotw : public QObject
{
    Q_OBJECT
public:
    explicit Lotw(QObject *parent = nullptr);

    QNetworkReply* update(const QDate &start_date, bool qso_since, const QString &stationCallsign);
    int uploadAdif(const QByteArray &data, QString &ErrorString);

    static const QString getUsername();
    static const QString getPassword();
    static const QString getTQSLPath(const QString &defaultPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    static void saveUsernamePassword(const QString&, const QString&);
    static void saveTQSLPath(const QString&);

signals:
    void updateProgress(int value);
    void updateStarted();
    void updateComplete(QSLMergeStat update);
    void updateFailed(QString);

public slots:
    void processReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* nam;
    static const QString SECURE_STORAGE_KEY;
    static const QString CONFIG_USERNAME_KEY;

    QNetworkReply* get(QList<QPair<QString, QString>> params);
};

#endif // LOTW_H
