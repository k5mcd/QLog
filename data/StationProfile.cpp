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
        << v.sota << v.sig << v.sigInfo << v.vucc
        << v.wwff;
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
    in >> v.wwff;

    return in;
}

StationProfilesManager::StationProfilesManager(QObject *parent) :
     QObject(parent),ProfileManager<StationProfile>("station")
{
    FCT_IDENTIFICATION;

    QSettings settings;

    QSqlQuery profileQuery;

    if ( ! profileQuery.prepare("SELECT profile_name, callsign, locator, operator_name, qth_name, iota, sota, sig, sig_info, vucc FROM station_profiles") )
    {
        qWarning()<< "Cannot prepare select";
    }

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

            addProfile(profileDB.profileName, profileDB);
        }
    }
    else
    {
        qInfo() << "Station Profile DB select error " << profileQuery.lastError().text();
    }

    /* TODO: remove this line in the future */
    settings.remove("station/profiles");
    settings.remove("station/currentprofile");
}

StationProfilesManager *StationProfilesManager::instance()
{
    FCT_IDENTIFICATION;

    static StationProfilesManager instance;
    return &instance;
}

void StationProfilesManager::save()
{
    FCT_IDENTIFICATION;
    QSettings settings;

    QSqlQuery deleteQuery;
    QSqlQuery insertQuery;

    if ( ! deleteQuery.prepare("DELETE FROM station_profiles") )
    {
        qWarning() << "cannot prepare Delete statement";
        return;
    }

    if ( ! insertQuery.prepare("INSERT INTO station_profiles(profile_name, callsign, locator, operator_name, qth_name, iota, sota, sig, sig_info, vucc, wwff) "
                        "VALUES (:profile_name, :callsign, :locator, :operator_name, :qth_name, :iota, :sota, :sig, :sig_info, :vucc, :wwff)") )
    {
        qWarning() << "cannot prepare Insert statement";
        return;
    }

    if ( deleteQuery.exec() )
    {
        auto keys = profileNameList();
        for ( auto &key: qAsConst(keys) )
        {
            StationProfile stationProfile = getProfile(key);

            insertQuery.bindValue(":profile_name", key);
            insertQuery.bindValue(":callsign", stationProfile.callsign);
            insertQuery.bindValue(":locator", stationProfile.locator);
            insertQuery.bindValue(":operator_name", stationProfile.operatorName);
            insertQuery.bindValue(":qth_name", stationProfile.qthName);
            insertQuery.bindValue(":iota", stationProfile.iota);
            insertQuery.bindValue(":sota", stationProfile.sota);
            insertQuery.bindValue(":sig", stationProfile.sig);
            insertQuery.bindValue(":sig_info", stationProfile.sigInfo);
            insertQuery.bindValue(":vucc", stationProfile.vucc);
            insertQuery.bindValue(":wwff", stationProfile.wwff);

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

    saveCurProfile1();
}

bool StationProfile::operator==(const StationProfile &profile)
{
    return (profile.profileName == this->profileName
            && profile.callsign == this->callsign
            && profile.locator == this->locator
            && profile.operatorName == this->operatorName
            && profile.qthName == this->qthName
            && profile.iota == this->iota
            && profile.sota == this->sota
            && profile.sig == this->sig
            && profile.sigInfo == this->sigInfo
            && profile.vucc == this->vucc
            && profile.wwff == this->wwff);
}

bool StationProfile::operator!=(const StationProfile &profile)
{
    return !operator==(profile);
}
