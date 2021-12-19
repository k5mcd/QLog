#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QtXml>
#include <QDebug>
#include "QRZ.h"
#include <QMessageBox>
#include "debug.h"
#include "core/CredentialStore.h"
#include "logformat/AdiFormat.h"

#define API_URL "https://xmldata.qrz.com/xml/current/"
#define API_LOGBOOK_URL "https://logbook.qrz.com/api"

MODULE_IDENTIFICATION("qlog.core.qrz");

QRZ::QRZ(QObject* parent) :
    GenericCallbook(parent),
    incorrectLogin(false),
    lastSeenPassword(QString()),
    cancelUpload(false),
    lastUploadReply(nullptr)
{
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished,
            this, &QRZ::processReply);
}

QRZ::~QRZ()
{
    nam->deleteLater();
}

void QRZ::queryCallsign(QString callsign)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<< callsign;

    if (sessionId.isEmpty()) {
        queuedCallsign = callsign;
        authenticate();
        return;
    }

    queuedCallsign = "";

    QUrlQuery query;
    query.addQueryItem("s", sessionId);
    query.addQueryItem("callsign", callsign);

    QUrl url(API_URL);
    url.setQuery(query);

    QNetworkReply *reply = nam->get(QNetworkRequest(url));

    reply->setProperty("queryCallsign", QVariant(callsign));
    reply->setProperty("messageType", QVariant("callsignInfoQuery"));
}

void QRZ::cancelUploadContacts()
{
    FCT_IDENTIFICATION;

    cancelUpload = true;
    if ( lastUploadReply )
    {
        lastUploadReply->abort();
    }
}


QNetworkReply *QRZ::uploadContact(const QSqlRecord record)
{
    FCT_IDENTIFICATION;

    //qCDebug(function_parameters) << record;

    QByteArray data;
    QTextStream stream(&data, QIODevice::ReadWrite);

    AdiFormat adi(stream);
    adi.exportContact(record);
    stream.flush();

    cancelUpload = false;
    QNetworkReply *reply = actionInsert(data, "REPLACE");
    reply->setProperty("contactID", record.value("id"));

    return reply;
}

QNetworkReply *QRZ::actionInsert(QByteArray& data, QString insertPolicy)
{
    FCT_IDENTIFICATION;

    QString username = getUsername();
    QString logbookAPIKey = getLogbookAPIKey();

    QUrlQuery params;
    params.addQueryItem("KEY", logbookAPIKey);
    params.addQueryItem("ACTION", "INSERT");
    params.addQueryItem("OPTION", insertPolicy);
    params.addQueryItem("ADIF", data.trimmed());

    QUrl url(API_LOGBOOK_URL);

    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    qCDebug(runtime) << url;

    QNetworkReply *reply = nam->post(request, params.query(QUrl::FullyEncoded).toUtf8());

    reply->setProperty("messageType", QVariant("actionsInsert"));

    return reply;
}


void QRZ::uploadContacts(const QList<QSqlRecord> qsos)
{
    FCT_IDENTIFICATION;

    //qCDebug(function_parameters) << qsos;

    if ( qsos.isEmpty() )
    {
        /* Nothing to do */
        emit uploadFinished(false);
        return;
    }

    cancelUpload = false;
    queuedContacts4Upload = qsos;

    lastUploadReply = uploadContact(queuedContacts4Upload.first());
    queuedContacts4Upload.removeFirst();
}

const QString QRZ::getUsername()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(QRZ::CONFIG_USERNAME_KEY).toString();
}

const QString QRZ::getPassword()
{
    FCT_IDENTIFICATION;

    return CredentialStore::instance()->getPassword(QRZ::SECURE_STORAGE_KEY,
                                                    getUsername());

}

const QString QRZ::getLogbookAPIKey()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return CredentialStore::instance()->getPassword(QRZ::SECURE_STORAGE_API_KEY,
                                        settings.value(QRZ::CONFIG_USERNAME_API_KEY,
                                                       QRZ::CONFIG_USERNAME_API_CONST).toString());
}

void QRZ::saveUsernamePassword(const QString newUsername, const QString newPassword)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    QString oldUsername = getUsername();
    if ( oldUsername != newUsername )
    {
        CredentialStore::instance()->deletePassword(QRZ::SECURE_STORAGE_KEY,
                                                    oldUsername);
    }

    settings.setValue(QRZ::CONFIG_USERNAME_KEY, newUsername);

    CredentialStore::instance()->savePassword(QRZ::SECURE_STORAGE_KEY,
                                              newUsername,
                                              newPassword);
}

void QRZ::saveLogbookAPI(const QString newKey)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue(QRZ::CONFIG_USERNAME_API_KEY, QRZ::CONFIG_USERNAME_API_CONST);

    CredentialStore::instance()->deletePassword(QRZ::SECURE_STORAGE_API_KEY,
                                                QRZ::CONFIG_USERNAME_API_CONST);

    if ( ! newKey.isEmpty() )
    {
        CredentialStore::instance()->savePassword(QRZ::SECURE_STORAGE_API_KEY,
                                                  QRZ::CONFIG_USERNAME_API_CONST,
                                                  newKey);
    }

}

void QRZ::authenticate()
{
    FCT_IDENTIFICATION;

    QString username = getUsername();
    QString password = getPassword();

    if ( incorrectLogin && password == lastSeenPassword)
    {
        queuedCallsign = QString();
        return;
    }

    if (!username.isEmpty() && !password.isEmpty()) {
        QUrlQuery query;
        query.addQueryItem("username", username);
        query.addQueryItem("password", password);
        query.addQueryItem("agent", "QLog");

        QUrl url(API_URL);
        url.setQuery(query);

        QNetworkReply *reply = nam->get(QNetworkRequest(url));
        reply->setProperty("messageType", QVariant("authenticate"));
        lastSeenPassword = password;
    }
    else
    {
        qCDebug(runtime) << "Empty username or password";
    }
}

void QRZ::processReply(QNetworkReply* reply) {
    FCT_IDENTIFICATION;

    if ( reply->error() != QNetworkReply::NoError )
    {
        qCDebug(runtime) << "QRZ.com error URL " << reply->request().url().toString();
        qCDebug(runtime) << "QRZ.com error" << reply->errorString();

        if ( reply->error() != QNetworkReply::OperationCanceledError )
        {
            emit uploadError(reply->errorString());
            emit lookupError(reply->errorString());
            reply->deleteLater();
        }

        lastUploadReply = nullptr;
        cancelUpload = true;
        return;
    }

    QString messageType = reply->property("messageType").toString();

    qCDebug(runtime) << "Received Message Type: " << messageType;

    /*********************/
    /* callsignInfoQuery */
    /*********************/
    if ( messageType == "callsignInfoQuery"
         || messageType == "authenticate" )
    {
        QByteArray response = reply->readAll();
        qCDebug(runtime) << response;
        QXmlStreamReader xml(response);

        /* Reset Session Key */
        /* Every response contains a valid key. If the key is not present */
        /* then it is needed to request a new one */

        sessionId = QString();

        QMap<QString, QString> data;

        while ( !xml.atEnd() && !xml.hasError() )
        {
            QXmlStreamReader::TokenType token = xml.readNext();

            if (token != QXmlStreamReader::StartElement) {
                continue;
            }

            if (xml.name() == "Error")
            {
                queuedCallsign = QString();
                QString errorString = xml.readElementText();

                if ( errorString.contains("Username/password incorrect"))
                {
                    incorrectLogin = true;
                    emit loginFailed();
                }
                else if ( errorString.contains("Not found:") )
                {
                    incorrectLogin = false;
                    emit callsignNotFound(reply->property("queryCallsign").toString());
                    //return;
                }
                else
                {
                    qInfo() << "QRZ Error - " << errorString;
                    emit lookupError(errorString);
                }

                // do not call return here, we need to obtain Key from error message (if present)
            }
            else
            {
                incorrectLogin = false;
            }

            if (xml.name() == "Key") {
                sessionId = xml.readElementText();
            }
            else if (xml.name() == "call") {
                data["call"] = xml.readElementText().toUpper();
            }
            else if (xml.name() == "dxcc") {
                data["dxcc"] = xml.readElementText();
            }
            else if (xml.name() == "fname") {
                data["fname"] = xml.readElementText();
            }
            else if (xml.name() == "name") {
                data["lname"] = xml.readElementText();
            }
            else if (xml.name() == "addr1") {
                data["addr1"] = xml.readElementText();
            }
            else if (xml.name() == "addr2") {
                data["qth"] = xml.readElementText();
            }
            else if (xml.name() == "state") {
                data["us_state"] = xml.readElementText();
            }
            else if (xml.name() == "zip") {
                data["zipcode"] = xml.readElementText();
            }
            else if (xml.name() == "country") {
                data["country"] = xml.readElementText();
            }
            else if (xml.name() == "lat") {
                data["latitude"] = xml.readElementText();
            }
            else if (xml.name() == "lon") {
                data["longitude"] = xml.readElementText();
            }
            else if (xml.name() == "county") {
                data["county"] = xml.readElementText();
            }
            else if (xml.name() == "grid") {
                data["gridsquare"] = xml.readElementText().toUpper();
            }
            else if (xml.name() == "efdate") {
                data["lic_year"] = xml.readElementText();
            }
            else if (xml.name() == "qslmgr") {
                data["qsl_via"] = xml.readElementText();
            }
            else if (xml.name() == "email") {
                data["email"] = xml.readElementText();
            }
            else if (xml.name() == "GMTOffset") {
                data["utc_offset"] = xml.readElementText();
            }
            else if (xml.name() == "eqsl") {
                data["eqsl"] = xml.readElementText();
            }
            else if (xml.name() == "mqsl") {
                data["pqsl"] = xml.readElementText();
            }
            else if (xml.name() == "cqzone") {
                data["cqz"] = xml.readElementText();
            }
            else if (xml.name() == "ituzone") {
                data["ituz"] = xml.readElementText();
            }
            else if (xml.name() == "born") {
                data["born"] = xml.readElementText();
            }
            else if (xml.name() == "lotw") {
                data["lotw"] = xml.readElementText();
            }
            else if (xml.name() == "iota") {
                data["iota"] = xml.readElementText();
            }
            else if (xml.name() == "nickname") {
                data["name"] = xml.readElementText();
            }
            else if (xml.name() == "url") {
                data["url"] = xml.readElementText();
            }

        }

        if (data.size())
        {
            emit callsignResult(data);
        }

        if (!queuedCallsign.isEmpty())
        {
            queryCallsign(queuedCallsign);
        }
    }
    /*****************/
    /* actionsInsert */
    /*****************/
    else if ( messageType == "actionsInsert")
    {
         QString replayString(reply->readAll());
         qCDebug(runtime) << replayString;

         QMap<QString, QString> data = parseActionResponse(replayString);

         QString status = data.value("RESULT", "FAILED");

         if ( status == "OK" || status == "REPLACE" )
         {
             qCDebug(runtime) << "Confirmed Upload for QSO Id " << reply->property("contactID").toInt();
             emit uploadedQSO(reply->property("contactID").toInt());

             if ( queuedContacts4Upload.isEmpty() )
             {
                 lastUploadReply = nullptr;
                 cancelUpload = false;
                 emit uploadFinished(true);
             }
             else
             {
                 if ( ! cancelUpload )
                 {
                     lastUploadReply = uploadContact(queuedContacts4Upload.first());
                     queuedContacts4Upload.removeFirst();
                 }
                 else
                 {
                     lastUploadReply = nullptr;
                 }
             }
         }
         else
         {
             emit uploadError(data.value("REASON", tr("General Error")));
             lastUploadReply = nullptr;
             cancelUpload = false;
         }
    }

    reply->deleteLater();
}

QMap<QString, QString> QRZ::parseActionResponse(const QString reponseString)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << reponseString;

    QStringList parsedResponse;
    QMap<QString, QString> data;

    parsedResponse << reponseString.split("&");

    for (auto param : parsedResponse)
    {
        QStringList parsedParams;
        parsedParams << param.split("=");

        if ( parsedParams.count() == 1 )
        {
            data[parsedParams.at(0)] = QString();
        }
        else if ( parsedParams.count() >= 2 )
        {
            data[parsedParams.at(0)] = parsedParams.at(1);
        }
    }

    return data;
}

const QString QRZ::SECURE_STORAGE_KEY = "QLog:QRZCOM";
const QString QRZ::SECURE_STORAGE_API_KEY = "QLog:QRZCOMAPI";
const QString QRZ::CONFIG_USERNAME_KEY = "qrzcom/username";
const QString QRZ::CONFIG_USERNAME_API_KEY = "qrzcom/usernameapi";
const QString QRZ::CONFIG_USERNAME_API_CONST = "logbookapi";
const QString QRZ::CALLBOOK_NAME = "qrzcom";
