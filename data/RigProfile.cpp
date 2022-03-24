#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>

#include "RigProfile.h"
#include "core/debug.h"
#include "data/ProfileManager.h"

MODULE_IDENTIFICATION("qlog.data.rigprofile");

QDataStream& operator<<(QDataStream& out, const RigProfile& v)
{

    out << v.profileName << v.model << v.portPath
        << v.hostname << v.netport << v.baudrate
        << v.databits << v.stopbits << v.flowcontrol
        << v.parity;

    return out;
}

QDataStream& operator>>(QDataStream& in, RigProfile& v)
{
    in >> v.profileName;
    in >> v.model;
    in >> v.portPath;
    in >> v.hostname;
    in >> v.netport;
    in >> v.baudrate;
    in >> v.databits;
    in >> v.stopbits;
    in >> v.flowcontrol;
    in >> v.parity;

    return in;
}

RigProfilesManager::RigProfilesManager(QObject *parent) :
    QObject(parent),
    ProfileManager<RigProfile>("equipment/rig")
{
    FCT_IDENTIFICATION;

    QSettings settings;

    QSqlQuery profileQuery;

    if ( ! profileQuery.prepare("SELECT profile_name, model, port_pathname, hostname, netport, baudrate, databits, stopbits, flowcontrol, parity FROM rig_profiles") )
    {
        qWarning()<< "Cannot prepare select";
    }

    if ( profileQuery.exec() )
    {
        while (profileQuery.next())
        {
            RigProfile profileDB;
            profileDB.profileName = profileQuery.value(0).toString();
            profileDB.model =  profileQuery.value(1).toInt();
            profileDB.portPath =  profileQuery.value(2).toString();
            profileDB.hostname =  profileQuery.value(3).toString();
            profileDB.netport =  profileQuery.value(4).toUInt();
            profileDB.baudrate =  profileQuery.value(5).toUInt();
            profileDB.databits =  profileQuery.value(6).toUInt();
            profileDB.stopbits =  profileQuery.value(7).toFloat();
            profileDB.flowcontrol =  profileQuery.value(8).toString();
            profileDB.parity =  profileQuery.value(9).toString();

            addProfile(profileDB.profileName, profileDB);
        }
    }
    else
    {
        qInfo() << "Station Profile DB select error " << profileQuery.lastError().text();
    }

    /* TODO: remove this line in the future */
    settings.remove("hamlib/rig/baudrate");
    settings.remove("hamlib/rig/databits");
    settings.remove("hamlib/rig/flowcontrol");
    settings.remove("hamlib/rig/hostname");
    settings.remove("hamlib/rig/model");
    settings.remove("hamlib/rig/modelrow");
    settings.remove("hamlib/rig/netport");
    settings.remove("hamlib/rig/parity");
    settings.remove("hamlib/rig/port");
    settings.remove("hamlib/rig/stopbits");
    settings.remove("station/rigs");
    settings.remove("newcontact/rig");
}

RigProfilesManager *RigProfilesManager::instance()
{
    FCT_IDENTIFICATION;

    static RigProfilesManager instance;
    return &instance;
}

void RigProfilesManager::save()
{
    FCT_IDENTIFICATION;

    QSqlQuery deleteQuery;
    QSqlQuery insertQuery;

    if ( ! deleteQuery.prepare("DELETE FROM rig_profiles") )
    {
        qWarning() << "cannot prepare Delete statement";
        return;
    }

    if ( ! insertQuery.prepare("INSERT INTO rig_profiles(profile_name, model, port_pathname, hostname, netport, baudrate, databits, stopbits, flowcontrol, parity) "
                        "VALUES (:profile_name, :model, :port_pathname, :hostname, :netport, :baudrate, :databits, :stopbits, :flowcontrol, :parity)") )
    {
        qWarning() << "cannot prepare Insert statement";
        return;
    }

    if ( deleteQuery.exec() )
    {
        auto keys = profileNameList();
        for ( auto &key: qAsConst(keys) )
        {
            RigProfile rigProfile = getProfile(key);

            insertQuery.bindValue(":profile_name", key);
            insertQuery.bindValue(":model", rigProfile.model);
            insertQuery.bindValue(":port_pathname", rigProfile.portPath);
            insertQuery.bindValue(":hostname", rigProfile.hostname);
            insertQuery.bindValue(":netport", rigProfile.netport);
            insertQuery.bindValue(":baudrate", rigProfile.baudrate);
            insertQuery.bindValue(":databits", rigProfile.databits);
            insertQuery.bindValue(":stopbits", rigProfile.stopbits);
            insertQuery.bindValue(":flowcontrol", rigProfile.flowcontrol);
            insertQuery.bindValue(":parity", rigProfile.parity);

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

bool RigProfile::operator==(const RigProfile &profile)
{
    return (profile.profileName == this->profileName
            && profile.model == this->model
            && profile.portPath == this->portPath
            && profile.hostname == this->hostname
            && profile.netport == this->netport
            && profile.baudrate == this->baudrate
            && profile.databits == this->databits
            && profile.stopbits == this->stopbits
            && profile.flowcontrol == this->flowcontrol
            && profile.parity == this->parity);
}

bool RigProfile::operator!=(const RigProfile &profile)
{
    return !operator==(profile);
}
