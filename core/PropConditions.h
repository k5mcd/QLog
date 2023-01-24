#ifndef PROPCONDITIONS_H
#define PROPCONDITIONS_H

#include <QObject>
#include <QDateTime>
#include <QVector>

class QNetworkAccessManager;
class QNetworkReply;

class AuroraMap: QObject
{
public:
    explicit AuroraMap(QObject *parent = nullptr) : QObject(parent){};
    ~AuroraMap(){};
    struct AuroraPoint
    {
        double longitude;
        double latitude;
        double propability;
    };

    void addPoint(double longitude, double latitude, double propability);
    QList<AuroraMap::AuroraPoint> getMap() const;
    void clear();
    QString lastForecastTime() const;
    int count() const;

private:
    QList<AuroraMap::AuroraPoint> auroraMap;
    QString forecastTime;
};

class PropConditions : public QObject
{
    Q_OBJECT
public:
    explicit PropConditions(QObject *parent = nullptr);
    ~PropConditions();
    bool isFluxValid();
    bool isKIndexValid();
    bool isAIndexValid();
    bool isAuroraMapValid();
    int getFlux();
    int getAIndex();
    double getKIndex();
    QList<AuroraMap::AuroraPoint> getAuroraPoints() const;

    static QString solarSummaryFile();

signals:
    void conditionsUpdated();
    void fluxUpdated();
    void KIndexUpdated();
    void AIndexUpdated();
    void auroraMapUpdated();

public slots:
    void update();
    void processReply(QNetworkReply* reply);

private:
    QDateTime flux_last_update;
    QDateTime k_index_last_update;
    QDateTime a_index_last_update;
    QDateTime auroraMap_last_update;
    int flux;
    int a_index;
    double k_index;
    AuroraMap auroraMap;

private:
    QNetworkAccessManager* nam;
};

#endif // PROPCONDITIONS_H
