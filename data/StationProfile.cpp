#include "data/StationProfile.h"
#include "core/debug.h"

#include <QSettings>
#include <QVariant>

MODULE_IDENTIFICATION("qlog.data.stationprofile");


QDataStream& operator<<(QDataStream& out, const StationProfile& v)
{
    out << v.profileName << v.callsign << v.locator << v.operatorName << v.qthName << v.iota << v.sota << v.sig << v.sigInfo << v.vucc;
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

    stationsProfiles = settings.value("station/profiles").toMap();
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

    settings.setValue("station/profiles", stationsProfiles);
    settings.setValue("station/currentprofile", currentProfile);
}

void StationProfilesManager::add(StationProfile profile)
{
    FCT_IDENTIFICATION;

    stationsProfiles.insert(profile.profileName, QVariant::fromValue(profile));
}

int StationProfilesManager::remove(QString profileName)
{
    FCT_IDENTIFICATION;

    if ( currentProfile == profileName )
    {
        currentProfile = QString();
    }

    return stationsProfiles.remove(profileName);
}

StationProfile StationProfilesManager::get(QString profileName)
{
    FCT_IDENTIFICATION;

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
