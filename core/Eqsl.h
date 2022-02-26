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
    ~EQSL();

    void update(QDate, QString);
    void uploadAdif(QByteArray &);
    void getQSLImage(QSqlRecord);

    static const QString getUsername();
    static const QString getPassword();
    static const QString getQSLImageFolder(const QString &defaultPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    static void saveUsernamePassword(const QString&, const QString&);
    static void saveQSLImageFolder(const QString&);

signals:
    void updateProgress(int value);
    void updateStarted();
    void updateComplete(QSLMergeStat);
    void updateFailed(QString);
    void uploadOK(QString);
    void uploadError(QString);
    void QSLImageFound(QString);
    void QSLImageError(QString);

public slots:
    void processReply(QNetworkReply*);
    void abortRequest();

private:
    QNetworkAccessManager* nam;

    void get(QList<QPair<QString, QString>>);
    void downloadADIF(const QString &);
    void downloadImage(const QString &, const QString &);
    QString QSLImageFilename(const QSqlRecord);
    bool isQSLImageInCache(QSqlRecord, QString &);
    QNetworkReply *currentReply;

    static const QString SECURE_STORAGE_KEY;
    static const QString CONFIG_USERNAME_KEY;
    static const QString CONFIG_QSL_FOLDER_KEY;

};

#endif // EQSL_H
