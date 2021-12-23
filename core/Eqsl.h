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

    QNetworkReply* update(QDate start_date, QString qthNick);
    QNetworkReply* uploadAdif(QByteArray &data);
    QNetworkReply* getQSLImage(QSqlRecord);

    static const QString getUsername();
    static const QString getPassword();
    static const QString getQSLImageFolder(const QString &defaultPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    static void saveUsernamePassword(const QString&, const QString&);
    static void saveQSLImageFolder(const QString&);

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

    QNetworkReply* get(QList<QPair<QString, QString>> params);
    void downloadADIF(const QString &filename);
    void downloadImage(const QString &filename, const QString  &futureFilename);
    QString QSLImageFilename(const QSqlRecord);
    bool isQSLImageInCache(QSqlRecord qso, QString &fullPath);

    static const QString SECURE_STORAGE_KEY;
    static const QString CONFIG_USERNAME_KEY;
    static const QString CONFIG_QSL_FOLDER_KEY;

};

#endif // EQSL_H
