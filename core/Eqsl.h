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
    void getQSLImage(QSqlRecord);
    static const QString SECURE_STORAGE_KEY;
    static const QString CONFIG_USERNAME_KEY;
    static const QString CONFIG_QSL_FOLDER_KEY;

signals:
    void updateProgress(int value);
    void updateStarted();
    void updateComplete(QSLMergeStat update);
    void updateFailed(QString error);
    void uploadOK(QString);
    void uploadError(QString);
    void QSLImageFound(QString);
    void QSLImageError(QString);

public slots:
    void processReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* nam;

    void get(QList<QPair<QString, QString>> params);
    void downloadADIF(QString filename);
    void downloadImage(QString filename, QString futureFilename);
    QString QSLImageFilename(QSqlRecord);
    bool isQSLImageInCache(QSqlRecord qso, QString &fullPath);


};

#endif // EQSL_H
