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
    QObject(parent)
{
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));
}

void HamQTH::queryCallsign(QString callsign) {
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

    nam->get(QNetworkRequest(url));
}

void HamQTH::authenticate() {
    FCT_IDENTIFICATION;

    QSettings settings;
    QString username = settings.value(HamQTH::CONFIG_USERNAME_KEY).toString();
    QString password = CredentialStore::instance()->getPassword(HamQTH::SECURE_STORAGE_KEY,
                                                                username);

    if (!username.isEmpty() && !password.isEmpty()) {
        QUrlQuery query;
        query.addQueryItem("u", username);
        query.addQueryItem("p", password);

        QUrl url(API_URL);
        url.setQuery(query);

        nam->get(QNetworkRequest(url));
    }
}

void HamQTH::processReply(QNetworkReply* reply) {
    FCT_IDENTIFICATION;

    if (reply->error() != QNetworkReply::NoError) {
        qCDebug(runtime) << "HamQTH error" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    qCDebug(runtime) << response;
    QXmlStreamReader xml(response);


    QMap<QString, QString> data;

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token != QXmlStreamReader::StartElement) {
            continue;
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
