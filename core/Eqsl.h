#ifndef EQSL_H
#define EQSL_H

#include <QObject>
#include <logformat/LogFormat.h>

class QNetworkAccessManager;
class QNetworkReply;

class EQSL : public QObject
{
    Q_OBJECT
public:
    explicit EQSL(QObject *parent = nullptr);

    void update(QDate start_date, QString qthNick);
    int uploadAdif(QByteArray &data);
    static const QString SECURE_STORAGE_KEY;
    static const QString CONFIG_USERNAME_KEY;

signals:
    void updateProgress(int value);
    void updateStarted();
    void updateComplete(QSLMergeStat update);
    void updateFailed(QString error);
    void uploadOK(QString);
    void uploadError(QString);

public slots:
    void processReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* nam;

    void get(QList<QPair<QString, QString>> params);
    void downloadADIF(QString filename);
};

#endif // EQSL_H
