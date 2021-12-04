#include "data/StationProfile.h"
#include "core/debug.h"

#include <QSettings>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>

MODULE_IDENTIFICATION("qlog.data.stationprofile");


QDataStream& operator<<(QDataStream& out, const StationProfile& v)
{
    out << v.profileName << v.callsign << v.locator
        << v.operatorName << v.qthName << v.iota
        << v.sota << v.sig << v.sigInfo << v.vucc;
    return out;
}

QDataStream& operator>>(QDataStream& in, StationProfile& v)
{
    in >> v.profileName;
    in >> v.callsign;
    in >> v.locator;
    in >> v.operatorName;
    in >> v.qthName;
    in >> v.iota;
    in >> v.sota;
    in >> v.sig;
    in >> v.sigInfo;
    in >> v.vucc;

    return in;
}

StationProfilesManager::StationProfilesManager(QObject *parent)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    QSqlQuery profileQuery;
    profileQuery.prepare("SELECT * FROM station_profiles");

    if ( profileQuery.exec() )
    {
        while (profileQuery.next())
        {
            StationProfile profileDB;
            profileDB.profileName = profileQuery.value(0).toString();
            profileDB.callsign =  profileQuery.value(1).toString();
            profileDB.locator =  profileQuery.value(2).toString();
            profileDB.operatorName =  profileQuery.value(3).toString();
            profileDB.qthName =  profileQuery.value(4).toString();
            profileDB.iota =  profileQuery.value(5).toString();
            profileDB.sota =  profileQuery.value(6).toString();
            profileDB.sig =  profileQuery.value(7).toString();
            profileDB.sigInfo =  profileQuery.value(8).toString();
            profileDB.vucc =  profileQuery.value(9).toString();
            add(profileDB);
        }
    }
    else
    {
        qInfo() << "Station Profile DB select error " << profileQuery.lastError().text();
    }

    /* TODO: remove this line in the future */
    settings.remove("station/profiles");

    currentProfile = settings.value("station/currentprofile", QString()).toString();
}

StationProfilesManager *StationProfilesManager::instance()
{
    FCT_IDENTIFICATION;

    static StationProfilesManager instance;
    return &instance;
}

QStringList StationProfilesManager::profilesList()
{
    FCT_IDENTIFICATION;

    QStringList ret;

    for ( auto key : stationsProfiles.keys() )
    {
        ret << key;
    }

    return ret;
}

void StationProfilesManager::save()
{
    FCT_IDENTIFICATION;
    QSettings settings;

    QSqlQuery deleteQuery;
    QSqlQuery insertQuery;

    deleteQuery.prepare("DELETE FROM station_profiles");
    insertQuery.prepare("INSERT INTO station_profiles(profile_name, callsign, locator, operator_name, qth_name, iota, sota, sig, sig_info, vucc) "
                        "VALUES (:profile_name, :callsign, :locator, :operator_name, :qth_name, :iota, :sota, :sig, :sig_info, :vucc)");

    if ( deleteQuery.exec() )
    {
        for ( auto key: stationsProfiles.keys() )
        {
            insertQuery.bindValue(":profile_name", key);
            insertQuery.bindValue(":callsign", stationsProfiles.value(key).value<StationProfile>().callsign);
            insertQuery.bindValue(":locator", stationsProfiles.value(key).value<StationProfile>().locator);
            insertQuery.bindValue(":operator_name", stationsProfiles.value(key).value<StationProfile>().operatorName);
            insertQuery.bindValue(":qth_name", stationsProfiles.value(key).value<StationProfile>().qthName);
            insertQuery.bindValue(":iota", stationsProfiles.value(key).value<StationProfile>().iota);
            insertQuery.bindValue(":sota", stationsProfiles.value(key).value<StationProfile>().sota);
            insertQuery.bindValue(":sig", stationsProfiles.value(key).value<StationProfile>().sig);
            insertQuery.bindValue(":sig_info", stationsProfiles.value(key).value<StationProfile>().sigInfo);
            insertQuery.bindValue(":vucc", stationsProfiles.value(key).value<StationProfile>().vucc);
            if ( ! insertQuery.exec() )
            {
                qInfo() << "Station Profile DB insert error " << insertQuery.lastError().text() << insertQuery.lastQuery();
            }
        }
    }
    else
    {
        qInfo() << "Station Profile DB delete error " << deleteQuery.lastError().text();
    }

    settings.setValue("station/currentprofile", currentProfile);
}

void StationProfilesManager::add(StationProfile profile)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << profile.profileName;

    stationsProfiles.insert(profile.profileName, QVariant::fromValue(profile));
}

int StationProfilesManager::remove(QString profileName)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << profileName;

    if ( currentProfile == profileName )
    {
        currentProfile = QString();
    }

    return stationsProfiles.remove(profileName);
}

StationProfile StationProfilesManager::get(QString profileName)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << profileName;

    if ( stationsProfiles.contains(profileName) )
    {
        return stationsProfiles.value(profileName).value<StationProfile>();
    }
    else
    {
        qCWarning(runtime) << "Profile " << profileName << " not found";
        return StationProfile();
    }
}

void StationProfilesManager::setCurrent(QString profileName)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    qCDebug(function_parameters) << profileName;

    currentProfile = profileName;
    settings.setValue("station/currentprofile", profileName);
}

StationProfile StationProfilesManager::getCurrent()
{
    FCT_IDENTIFICATION;

    if ( ! currentProfile.isEmpty() )
    {
        return get(currentProfile);
    }
    else
    {
        return StationProfile();
    }
}
