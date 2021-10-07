#ifndef LOTW_H
#define LOTW_H

#include <QObject>
#include <logformat/LogFormat.h>

class QNetworkAccessManager;
class QNetworkReply;

class Lotw : public QObject
{
    Q_OBJECT
public:
    explicit Lotw(QObject *parent = nullptr);

    void update(QDate start_date, bool qso_since, QString stationCallsign);
    int uploadAdif(QByteArray &data, QString &ErrorString);
    static const QString SECURE_STORAGE_KEY;
    static const QString CONFIG_USERNAME_KEY;

signals:
    void updateProgress(int value);
    void updateStarted();
    void updateComplete(QSLMergeStat update);
    void updateFailed(QString);

public slots:
    void processReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* nam;

    void get(QList<QPair<QString, QString>> params);
};

#endif // LOTW_H
