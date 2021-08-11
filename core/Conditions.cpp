#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "Conditions.h"
#include "debug.h"

const QUrl FLUX_URL = QUrl("https://services.swpc.noaa.gov/products/summary/10cm-flux.json");
const QUrl K_INDEX_URL = QUrl("https://services.swpc.noaa.gov/products/noaa-planetary-k-index.json");

MODULE_IDENTIFICATION("qlog.core.conditions");

Conditions::Conditions(QObject *parent) : QObject(parent)
{
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Conditions::update);
    timer->start(15*60*1000);
}

void Conditions::update() {
    FCT_IDENTIFICATION;

    nam->get(QNetworkRequest(FLUX_URL));
    nam->get(QNetworkRequest(K_INDEX_URL));
}

void Conditions::processReply(QNetworkReply* reply) {
    FCT_IDENTIFICATION;

    QByteArray data = reply->readAll();

    qCDebug(runtime) << data;

    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (reply->url() == FLUX_URL) {
            QVariantMap obj = doc.object().toVariantMap();
            flux = obj.value("Flux").toInt();
        }
        else if (reply->url() == K_INDEX_URL) {
            k_index = doc.array().last().toArray().at(2).toString().toDouble();
        }
        reply->deleteLater();
        emit conditionsUpdated();
    }
    else {
        reply->deleteLater();
    }
}

Conditions::~Conditions() {
    delete nam;
}
