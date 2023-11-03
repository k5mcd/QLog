#ifndef EQSL_H
#define EQSL_H

#include <QObject>
#include <logformat/LogFormat.h>

#include "QSLStorage.h"

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
    void getQSLImage(const QSqlRecord&);

    static const QString getUsername();
    static const QString getPassword();
    static void saveUsernamePassword(const QString&, const QString&);

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
    QSLStorage *qslStorage;

    void get(QList<QPair<QString, QString>>);
    void downloadADIF(const QString &);
    void downloadImage(const QString &, const QString &, const qulonglong);
    QString QSLImageFilename(const QSqlRecord &);
    bool isQSLImageInCache(const QSqlRecord &, QString &);
    QNetworkReply *currentReply;

    static const QString SECURE_STORAGE_KEY;
    static const QString CONFIG_USERNAME_KEY;

};

#endif // EQSL_H
