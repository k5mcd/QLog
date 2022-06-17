#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QtXml>
#include <QDebug>
#include "HamQTH.h"
#include "debug.h"
#include "core/CredentialStore.h"

#define API_URL "http://www.hamqth.com/xml.php"

MODULE_IDENTIFICATION("qlog.core.hamqth");

HamQTH::HamQTH(QObject* parent) :
    GenericCallbook(parent),
    currentReply(nullptr)
{
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished,
            this, &HamQTH::processReply);

    incorrectLogin = false;
    lastSeenPassword = "";
}

HamQTH::~HamQTH()
{
    if ( currentReply )
    {
        currentReply->abort();
        currentReply->deleteLater();
    }

    nam->deleteLater();
}

const QString HamQTH::getUsername()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(HamQTH::CONFIG_USERNAME_KEY).toString();

}

const QString HamQTH::getPassword()
{
    FCT_IDENTIFICATION;

    return CredentialStore::instance()->getPassword(HamQTH::SECURE_STORAGE_KEY,
                                                   getUsername());
}

void HamQTH::saveUsernamePassword(const QString &newUsername, const QString &newPassword)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    QString oldUsername = getUsername();
    if ( oldUsername != newUsername )
    {
        CredentialStore::instance()->deletePassword(HamQTH::SECURE_STORAGE_KEY,
                                                    oldUsername);
    }

    settings.setValue(HamQTH::CONFIG_USERNAME_KEY, newUsername);

    CredentialStore::instance()->savePassword(HamQTH::SECURE_STORAGE_KEY,
                                              newUsername,
                                              newPassword);

}

QString HamQTH::getDisplayName()
{
    FCT_IDENTIFICATION;
    return QString(tr("HamQTH"));
}

void HamQTH::queryCallsign(QString callsign)
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
    query.addQueryItem("id", sessionId);
    query.addQueryItem("callsign", callsign);
    query.addQueryItem("prg", "QLog");

    QUrl url(API_URL);
    url.setQuery(query);

    if ( currentReply )
    {
        qCWarning(runtime) << "processing a new request but the previous one hasn't been completed yet !!!";
    }

    currentReply = nam->get(QNetworkRequest(url));
    currentReply->setProperty("queryCallsign", callsign);
}

void HamQTH::abortQuery()
{
    FCT_IDENTIFICATION;

    if ( currentReply )
    {
        currentReply->abort();
        //currentReply->deleteLater(); // pointer is deleted later in processReply
        currentReply = nullptr;
    }

}

void HamQTH::authenticate() {
    FCT_IDENTIFICATION;

    QSettings settings;
    QString username = settings.value(HamQTH::CONFIG_USERNAME_KEY).toString();
    QString password = CredentialStore::instance()->getPassword(HamQTH::SECURE_STORAGE_KEY,
                                                                username);

    if ( incorrectLogin && password == lastSeenPassword)
    {
        /* User already knows that login failed */
        emit callsignNotFound(queuedCallsign);
        queuedCallsign = QString();
        return;
    }

    if (!username.isEmpty() && !password.isEmpty()) {
        QUrlQuery query;
        query.addQueryItem("u", username);
        query.addQueryItem("p", password);

        QUrl url(API_URL);
        url.setQuery(query);

        if ( currentReply )
        {
            qCWarning(runtime) << "processing a new request but the previous one hasn't been completed yet !!!";
        }

        currentReply = nam->get(QNetworkRequest(url));
        lastSeenPassword = password;
        qCDebug(runtime) << "Sent Auth message";
    }
    else
    {
        qCDebug(runtime) << "Empty username or password";
    }
}

void HamQTH::processReply(QNetworkReply* reply) {
    FCT_IDENTIFICATION;

    /* always process one requests per class */
    currentReply = nullptr;

    if (reply->error() != QNetworkReply::NoError)
    {
        qInfo() << "HamQTH error" << reply->errorString();
        emit lookupError(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    qCDebug(runtime) << response;
    QXmlStreamReader xml(response);


    QMap<QString, QString> data;

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token != QXmlStreamReader::StartElement) {
            continue;
        }

        if (xml.name() == "error")
        {
            queuedCallsign = QString();
            QString errorString = xml.readElementText();

            if ( errorString == "Wrong user name or password" )
            {
                incorrectLogin = true;
                emit loginFailed();
            }
            else if ( errorString == "Callsign not found" )
            {
                incorrectLogin = false;
                emit callsignNotFound(reply->property("queryCallsign").toString());
                return;
            }
            else
            {
                qInfo() << "HamQTH Error - " << errorString;
            }
            sessionId = QString();
            emit lookupError(errorString);
            return;
        }
        else
        {
            incorrectLogin = false;
        }

        if (xml.name() == "session_id") {
            sessionId = xml.readElementText();
        }
        else if (xml.name() == "callsign") {
            data["call"] = xml.readElementText().toUpper();
        }
        else if (xml.name() == "nick") {
            data["name"] = xml.readElementText();
        }
        else if (xml.name() == "qth") {
            data["qth"] = xml.readElementText();
        }
        else if (xml.name() == "grid") {
            data["gridsquare"] = xml.readElementText().toUpper();
        }
        else if (xml.name() == "qsl_via") {
            data["qsl_via"] = xml.readElementText().toUpper();
        }
        else if (xml.name() == "cq") {
            data["cqz"] = xml.readElementText();
        }
        else if (xml.name() == "itu") {
            data["ituz"] = xml.readElementText();
        }
        else if (xml.name() == "dok") {
            data["dok"] = xml.readElementText().toUpper();
        }
        else if (xml.name() == "iota") {
            data["iota"] = xml.readElementText().toUpper();
        }
        else if (xml.name() == "email") {
            data["email"] = xml.readElementText();
        }
        else if (xml.name() == "adif") {
            data["dxcc"] = xml.readElementText();
        }
        else if (xml.name() == "addr_name") {
            data["lname"] = xml.readElementText();
        }
        else if (xml.name() == "adr_street1") {
            data["addr1"] = xml.readElementText();
        }
        else if (xml.name() == "us_state") {
            data["us_state"] = xml.readElementText();
        }
        else if (xml.name() == "adr_zip") {
            data["zipcode"] = xml.readElementText();
        }
        else if (xml.name() == "country") {
            data["country"] = xml.readElementText();
        }
        else if (xml.name() == "latitude") {
            data["latitude"] = xml.readElementText();
        }
        else if (xml.name() == "longitude") {
            data["longitude"] = xml.readElementText();
        }
        else if (xml.name() == "county") {
            data["county"] = xml.readElementText();
        }
        else if (xml.name() == "lic_year") {
            data["lic_year"] = xml.readElementText();
        }
        else if (xml.name() == "utc_offset") {
            data["utc_offset"] = xml.readElementText();
        }
        else if (xml.name() == "eqsl") {
            data["eqsl"] = xml.readElementText();
        }
        else if (xml.name() == "qsl") {
            data["pqsl"] = xml.readElementText();
        }
        else if (xml.name() == "birth_year") {
            data["born"] = xml.readElementText();
        }
        else if (xml.name() == "lotw") {
            data["lotw"] = xml.readElementText();
        }
        else if (xml.name() == "lotw") {
            data["lotw"] = xml.readElementText();
        }
        else if (xml.name() == "web") {
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

const QString HamQTH::SECURE_STORAGE_KEY = "QLog:HamQTH";
const QString HamQTH::CONFIG_USERNAME_KEY = "hamqth/username";
const QString HamQTH::CALLBOOK_NAME = "hamqth";
