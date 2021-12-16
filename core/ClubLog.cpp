#include <QSettings>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include "logformat/AdiFormat.h"
#include "ClubLog.h"
#include "debug.h"
#include "core/CredentialStore.h"

#define API_KEY "21507885dece41ca049fec7fe02a813f2105aff2"
#define API_LIVE_UPLOAD_URL "https://clublog.org/realtime.php"
#define API_LOG_UPLOAD_URL "https://clublog.org/putlogs.php"

MODULE_IDENTIFICATION("qlog.core.clublog");

ClubLog::ClubLog(QObject *parent) : QObject(parent) {
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));
}

void ClubLog::uploadContact(QSqlRecord record) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << record;

    QSettings settings;
    QString email = settings.value(ClubLog::CONFIG_EMAIL_KEY).toString();
    QString callsign = settings.value(ClubLog::CONFIG_CALLSIGN_KEY).toString();
    QString password = CredentialStore::instance()->getPassword(ClubLog::SECURE_STORAGE_KEY,
                                                                email);

    if (email.isEmpty() || callsign.isEmpty() || password.isEmpty()) {
        return;
    }

    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);

    qCDebug(runtime) << "Exporting to ADIF";
    AdiFormat adi(stream);
    adi.exportContact(record);
    stream.flush();
    qCDebug(runtime) << "Exported to ADIF";

    QUrlQuery query;
    query.addQueryItem("email", email);
    query.addQueryItem("callsign", callsign);
    query.addQueryItem("password", password);
    query.addQueryItem("api", API_KEY);
    query.addQueryItem("adif", data);

    QUrl url(API_LIVE_UPLOAD_URL);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    qCDebug(runtime) << query.query();

    nam->post(request, query.query().toUtf8());
}

QNetworkReply* ClubLog::uploadAdif(QByteArray& data)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << data;

    QSettings settings;
    QString email = settings.value(ClubLog::CONFIG_EMAIL_KEY).toString();
    QString callsign = settings.value(ClubLog::CONFIG_CALLSIGN_KEY).toString();
    QString password = CredentialStore::instance()->getPassword(ClubLog::SECURE_STORAGE_KEY,
                                                                email);

    QUrl url(API_LOG_UPLOAD_URL);

    QHttpMultiPart* multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart emailPart;
    emailPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"email\""));
    emailPart.setBody(email.toUtf8());

    QHttpPart callsignPart;
    callsignPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"callsign\""));
    callsignPart.setBody(callsign.toUtf8());

    QHttpPart passwordPart;
    passwordPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"password\""));
    passwordPart.setBody(password.toUtf8());

    QHttpPart clearPart;
    clearPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"clear\""));
    clearPart.setBody("0");

    QHttpPart apiPart;
    apiPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"api\""));
    apiPart.setBody(API_KEY);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"clublog.adi\""));
    filePart.setBody(data);

    multipart->append(emailPart);
    multipart->append(passwordPart);
    multipart->append(callsignPart);
    multipart->append(clearPart);
    multipart->append(apiPart);
    multipart->append(filePart);

    qCDebug(runtime) << multipart->boundary();

    QNetworkRequest request(url);
    QNetworkReply* reply = nam->post(request, multipart);
    reply->setProperty("messageType", QVariant("uploadADIFFile"));
    multipart->setParent(reply);

    return reply;
}

void ClubLog::processReply(QNetworkReply* reply)
{
    FCT_IDENTIFICATION;

    if ( reply->error() != QNetworkReply::NoError )
    {
        qCDebug(runtime) << "eQSL error URL " << reply->request().url().toString();
        qCDebug(runtime) << "eQSL error" << reply->errorString();
        if ( reply->error() != QNetworkReply::OperationCanceledError )
        {
            emit uploadError(reply->errorString());
            reply->deleteLater();
        }
        return;
    }

    QString messageType = reply->property("messageType").toString();

    qCDebug(runtime) << "Received Message Type: " << messageType;

    /******************/
    /* uploadADIFFile */
    /******************/
    if ( messageType == "uploadADIFFile" )
    {
        emit uploadOK("OK");
    }

    reply->deleteLater();
}

const QString ClubLog::SECURE_STORAGE_KEY = "QLog:Clublog";
const QString ClubLog::CONFIG_EMAIL_KEY = "clublog/email";
const QString ClubLog::CONFIG_CALLSIGN_KEY = "clublog/callsign";
