#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>

#include "AntProfile.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.data.antprofile");

QDataStream& operator<<(QDataStream& out, const AntProfile& v)
{

    out << v.profileName << v.description;
    return out;
}

QDataStream& operator>>(QDataStream& in, AntProfile& v)
{
    in >> v.profileName;
    in >> v.description;
    return in;
}

AntProfilesManager::AntProfilesManager(QObject *parent) :
    QObject(parent),
    ProfileManager<AntProfile>("equipment/ant")
{
    FCT_IDENTIFICATION;

    QSqlQuery profileQuery;

    if ( ! profileQuery.prepare("SELECT profile_name, desc FROM ant_profiles") )
    {
        qWarning()<< "Cannot prepare select";
    }

    if ( profileQuery.exec() )
    {
        while (profileQuery.next())
        {
            AntProfile profileDB;
            profileDB.profileName = profileQuery.value(0).toString();
            profileDB.description =  profileQuery.value(1).toInt();

            addProfile(profileDB.profileName, profileDB);
        }
    }
    else
    {
        qInfo() << "Station Profile DB select error " << profileQuery.lastError().text();
    }

    /* TODO: remove this line in the future */
    QSettings settings;
    settings.remove("newcontact/antenna");
    settings.remove("station/antennas");

}

AntProfilesManager *AntProfilesManager::instance()
{
    FCT_IDENTIFICATION;

    static AntProfilesManager instance;
    return &instance;

}

void AntProfilesManager::save()
{
    FCT_IDENTIFICATION;

    QSqlQuery deleteQuery;
    QSqlQuery insertQuery;

    if ( ! deleteQuery.prepare("DELETE FROM ant_profiles") )
    {
        qWarning() << "cannot prepare Delete statement";
        return;
    }

    if ( ! insertQuery.prepare("INSERT INTO ant_profiles(profile_name, desc) "
                        "VALUES (:profile_name, :desc)") )
    {
        qWarning() << "cannot prepare Insert statement";
        return;
    }

    if ( deleteQuery.exec() )
    {
        auto keys = profileNameList();
        for ( auto &key: qAsConst(keys) )
        {
            AntProfile antProfile = getProfile(key);

            insertQuery.bindValue(":profile_name", key);
            insertQuery.bindValue(":desc", antProfile.description);


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
