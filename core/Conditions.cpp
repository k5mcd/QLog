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
            flux_last_update = QDateTime::currentDateTime();
        }
        else if (reply->url() == K_INDEX_URL) {
            k_index = doc.array().last().toArray().at(2).toString().toDouble();
            k_index_last_update = QDateTime::currentDateTime();
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

bool Conditions::isFluxValid()
{
    FCT_IDENTIFICATION;
    bool ret = false;

    qCDebug(runtime)<<"Date valid: " << flux_last_update.isValid() << " last_update: " << flux_last_update;

    ret = (flux_last_update.isValid()
           && flux_last_update.secsTo(QDateTime::currentDateTime()) < 20 * 60);

    qCDebug(runtime)<< "Result: " << ret;
    return ret;
}

bool Conditions::isKIndexValid()
{
    FCT_IDENTIFICATION;
    bool ret = false;

    qCDebug(runtime)<<"Date valid: " << k_index_last_update.isValid() << " last_update: " << k_index_last_update;

    ret = (k_index_last_update.isValid()
           && k_index_last_update.secsTo(QDateTime::currentDateTime()) < 20 * 60);

    qCDebug(runtime)<< "Result: " << ret;

    return ret;
}

int Conditions::getFlux()
{
    FCT_IDENTIFICATION;
    qCDebug(runtime)<<"Current Flux: " << flux << " last_update: " << flux_last_update;
    return flux;

}

double Conditions::getKIndex()
{
    FCT_IDENTIFICATION;
    qCDebug(runtime)<<"Current K-Index: " << k_index << " last_update: " << k_index_last_update;
    return k_index;
}
