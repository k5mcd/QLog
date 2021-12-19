#ifndef STATIONPROFILE_H
#define STATIONPROFILE_H

#include <QMetaType>
#include <QString>
#include <QObject>
#include <QDataStream>
#include <QVariant>
#include <QMap>

class StationProfile
{

public:
    StationProfile() {};

    QString profileName;
    QString callsign;
    QString locator;
    QString operatorName;
    QString qthName;
    QString iota;
    QString sota;
    QString sig;
    QString sigInfo;
    QString vucc;

private:
    friend QDataStream& operator<<(QDataStream& out, const StationProfile& v);
    friend QDataStream& operator>>(QDataStream& in, StationProfile& v);
};

Q_DECLARE_METATYPE(StationProfile)

class StationProfilesManager : QObject
{
    Q_OBJECT

public:

    explicit StationProfilesManager(QObject *parent = nullptr);
    ~StationProfilesManager() {};

    static StationProfilesManager* instance();
    QStringList profilesList();
    void save();
    void add(StationProfile profile);
    int remove(QString profileName);
    StationProfile get(QString profileName);
    void setCurrent(QString profileName);
    StationProfile getCurrent();

private:
    QMap<QString, QVariant> stationsProfiles;
    QString currentProfile;
};


#endif // STATIONPROFILE_H
