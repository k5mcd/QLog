#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include "Conditions.h"
#include "debug.h"

//#define FLUX_URL "https://services.swpc.noaa.gov/products/summary/10cm-flux.json"
#define K_INDEX_URL "https://www.hamqsl.com/solarxml.php"
#define SOLAR_SUMMARY_IMG "https://www.hamqsl.com/solar101vhf.php"
#define AURORA_MAP "https://services.swpc.noaa.gov/json/ovation_aurora_latest.json"

MODULE_IDENTIFICATION("qlog.core.conditions");

Conditions::Conditions(QObject *parent) : QObject(parent)
{
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished, this, &Conditions::processReply);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Conditions::update);
    timer->start(15*60*1000);
}

void Conditions::update() {
    FCT_IDENTIFICATION;

    nam->get(QNetworkRequest(QUrl(SOLAR_SUMMARY_IMG)));
    nam->get(QNetworkRequest(QUrl(K_INDEX_URL)));
    nam->get(QNetworkRequest(QUrl(AURORA_MAP)));
}

void Conditions::processReply(QNetworkReply* reply) {
    FCT_IDENTIFICATION;

    QByteArray data = reply->readAll();

    qCDebug(runtime) << data;

    int replyStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->isFinished()
        && reply->error() == QNetworkReply::NoError
        && replyStatusCode >= 200 && replyStatusCode < 300)
    {
        if (reply->url() == QUrl(SOLAR_SUMMARY_IMG))
        {
            QFile file(solarSummaryFile());

            if ( file.open(QIODevice::WriteOnly))
            {
                file.write(data);
                file.flush();
                file.close();
            }
        }
        else if (reply->url() == QUrl(K_INDEX_URL))
        {
            QDomDocument doc;

            if ( !doc.setContent(data) )
            {
                qWarning() << "Cannot parse response from " << K_INDEX_URL;
                return;
            }

            QDomNodeList solarData = doc.elementsByTagName("solardata");
            QDomNode n = solarData.item(0);

            if ( n.isNull() )
            {
                qWarning() << "Cannot find solardata in " << K_INDEX_URL;
                return;
            }

            QDomElement aindex = n.firstChildElement("aindex");
            QDomElement kindex = n.firstChildElement("kindex");
            QDomElement solarflux = n.firstChildElement("solarflux");

            if ( !aindex.isNull() )
            {
                a_index = aindex.text().toInt();
                qCDebug(runtime) << "A-Index: " << a_index;
                a_index_last_update = QDateTime::currentDateTime();
                emit AIndexUpdated();
            }

            if ( !kindex.isNull() )
            {
                k_index = kindex.text().toDouble();
                qCDebug(runtime) << "K-Index: " << k_index;
                k_index_last_update = QDateTime::currentDateTime();
                emit KIndexUpdated();
            }

            if ( !solarflux.isNull() )
            {
                flux = solarflux.text().toInt();
                qCDebug(runtime) << "Flux: " << flux;
                flux_last_update = QDateTime::currentDateTime();
                emit fluxUpdated();
            }
        }
        else if (reply->url() == QUrl(AURORA_MAP))
        {
            auroraMap.clear();

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QString forecastTime = doc["Forecast Time"].toString();
            qCDebug(runtime) << "Aurora forecast Time:" << forecastTime;
            if ( forecastTime != auroraMap.lastForecastTime() )
            {
                QJsonArray jsonArray = doc["coordinates"].toArray();
                for (const QJsonValue &value : qAsConst(jsonArray))
                {
                    QJsonArray obj = value.toArray();
                    if ( obj.size() == 3 )
                    {
                        double longitute = obj[0].toDouble();
                        double latitude = obj[1].toDouble();
                        double prob = obj[2].toDouble();
                        auroraMap.addPoint(longitute - 360, latitude, prob);
                        auroraMap.addPoint(longitute, latitude, prob);
                    }
                }
                auroraMap_last_update = QDateTime::currentDateTime();
                emit auroraMapUpdated();
            }
            else
            {
                qCDebug(runtime) << "the same Aurora forecast - ignore";
            }
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

Conditions *Conditions::instance()
{
    FCT_IDENTIFICATION;
    static Conditions instance;
    return &instance;
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

bool Conditions::isAIndexValid()
{
    FCT_IDENTIFICATION;
    bool ret = false;

    qCDebug(runtime)<<"Date valid: " << a_index_last_update.isValid() << " last_update: " << a_index_last_update;

    ret = (a_index_last_update.isValid()
           && a_index_last_update.secsTo(QDateTime::currentDateTime()) < 20 * 60);

    qCDebug(runtime)<< "Result: " << ret;

    return ret;
}

bool Conditions::isAuroraMapValid()
{
    FCT_IDENTIFICATION;

    bool ret = false;

    qCDebug(runtime)<<"Date valid: " << auroraMap_last_update.isValid()
                    << " last_update: " << auroraMap_last_update
                    << " aurora count: " << auroraMap.count();

    ret = (auroraMap_last_update.isValid()
           && auroraMap_last_update.secsTo(QDateTime::currentDateTime()) < 20 * 60
           && auroraMap.count() > 0);

    qCDebug(runtime)<< "Result: " << ret;

    return ret;
}

int Conditions::getFlux()
{
    FCT_IDENTIFICATION;
    qCDebug(runtime)<<"Current Flux: " << flux << " last_update: " << flux_last_update;
    return flux;

}

int Conditions::getAIndex()
{
    FCT_IDENTIFICATION;
    qCDebug(runtime)<<"Current A-Index: " << a_index << " last_update: " << a_index_last_update;
    return a_index;
}

double Conditions::getKIndex()
{
    FCT_IDENTIFICATION;
    qCDebug(runtime)<<"Current K-Index: " << k_index << " last_update: " << k_index_last_update;
    return k_index;
}

QList<AuroraMap::AuroraPoint> Conditions::getAuroraPoints() const
{
    FCT_IDENTIFICATION;

    return auroraMap.getMap();
}

QString Conditions::solarSummaryFile()
{
    FCT_IDENTIFICATION;

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    return dir.filePath("solar101vhf.gif");
}

void AuroraMap::addPoint(double longitude, double latitude, double propability)
{
    FCT_IDENTIFICATION;

    if ( propability == 0 )
        return;

    AuroraMap::AuroraPoint point;
    point.longitude = longitude;
    point.latitude = latitude;
    point.propability = propability;

    auroraMap.append(point);
}

QList<AuroraMap::AuroraPoint> AuroraMap::getMap() const
{
    FCT_IDENTIFICATION;

    return auroraMap;
}

void AuroraMap::clear()
{
    FCT_IDENTIFICATION;

    auroraMap.clear();
}

QString AuroraMap::lastForecastTime() const
{
    FCT_IDENTIFICATION;

    return forecastTime;
}

int AuroraMap::count() const
{
    FCT_IDENTIFICATION;

    return auroraMap.size();
}
