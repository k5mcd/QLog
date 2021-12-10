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


#define API_URL "https://xmldata.qrz.com/xml/current/"

MODULE_IDENTIFICATION("qlog.core.qrz");

QRZ::QRZ(QObject* parent) :
    GenericCallbook(parent)
{
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished,
            this, &QRZ::processReply);

    incorrectLogin = false;
    lastSeenPassword = QString();
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

    nam->get(QNetworkRequest(url))->setProperty("queryCallsign", callsign);
}

void QRZ::authenticate() {
    FCT_IDENTIFICATION;

    QSettings settings;
    QString username = settings.value(QRZ::CONFIG_USERNAME_KEY).toString();
    QString password = CredentialStore::instance()->getPassword(QRZ::SECURE_STORAGE_KEY,
                                                                username);

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

        nam->get(QNetworkRequest(url));
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
        qInfo() << "QRZ error" << reply->errorString();
        emit lookupError(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    qCDebug(runtime) << response;
    QXmlStreamReader xml(response);

    /* Reset Session Key */
    /* Every response contains a valid key. If the key is not present
     * then it is needed to request a new one */

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
            sessionId = QString();
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
                return;
            }
            else
            {
                qInfo() << "QRZ Error - " << errorString;
            }
            emit lookupError(errorString);
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
            data["dxcc"] = xml.readElementText().toUpper();
        }
        else if (xml.name() == "fname") {
            data["fname"] = xml.readElementText().toUpper();
        }
        else if (xml.name() == "name") {
            data["lname"] = xml.readElementText().toUpper();
        }
        else if (xml.name() == "addr1") {
            data["addr1"] = xml.readElementText().toUpper();
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

    reply->deleteLater();

    if (data.size()) {
        emit callsignResult(data);
    }

    if (!queuedCallsign.isEmpty()) {
        queryCallsign(queuedCallsign);
    }
}

const QString QRZ::SECURE_STORAGE_KEY = "QLog:QRZCOM";
const QString QRZ::CONFIG_USERNAME_KEY = "qrzcom/username";
const QString QRZ::CALLBOOK_NAME = "qrzcom";
