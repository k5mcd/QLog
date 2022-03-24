#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>

#include "RotProfile.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.data.rotprofile");

QDataStream& operator<<(QDataStream& out, const RotProfile& v)
{

    out << v.profileName << v.model << v.portPath
        << v.hostname << v.netport << v.baudrate
        << v.databits << v.stopbits << v.flowcontrol
        << v.parity;

    return out;
}

QDataStream& operator>>(QDataStream& in, RotProfile& v)
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

RotProfilesManager::RotProfilesManager(QObject *parent) :
    QObject(parent),
    ProfileManager<RotProfile>("equipment/rot")
{
    FCT_IDENTIFICATION;

    QSettings settings;

    QSqlQuery profileQuery;

    if ( ! profileQuery.prepare("SELECT profile_name, model, port_pathname, hostname, netport, baudrate, databits, stopbits, flowcontrol, parity FROM rot_profiles") )
    {
        qWarning()<< "Cannot prepare select";
    }

    if ( profileQuery.exec() )
    {
        while (profileQuery.next())
        {
            RotProfile profileDB;
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
        qInfo() << "Rot Profile DB select error " << profileQuery.lastError().text();
    }

    /* TODO: remove this line in the future */
    settings.remove("hamlib/rot/baudrate");
    settings.remove("hamlib/rot/databits");
    settings.remove("hamlib/rot/flowcontrol");
    settings.remove("hamlib/rot/hostname");
    settings.remove("hamlib/rot/model");
    settings.remove("hamlib/rot/modelrow");
    settings.remove("hamlib/rot/netport");
    settings.remove("hamlib/rot/parity");
    settings.remove("hamlib/rot/port");
    settings.remove("hamlib/rot/stopbits");
}

RotProfilesManager *RotProfilesManager::instance()
{
    FCT_IDENTIFICATION;

    static RotProfilesManager instance;
    return &instance;
}

void RotProfilesManager::save()
{
    FCT_IDENTIFICATION;

    QSqlQuery deleteQuery;
    QSqlQuery insertQuery;

    if ( ! deleteQuery.prepare("DELETE FROM rot_profiles") )
    {
        qWarning() << "cannot prepare Delete statement";
        return;
    }

    if ( ! insertQuery.prepare("INSERT INTO rot_profiles(profile_name, model, port_pathname, hostname, netport, baudrate, databits, stopbits, flowcontrol, parity) "
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
            RotProfile rigProfile = getProfile(key);

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
        qInfo() << "Rot Profile DB delete error " << deleteQuery.lastError().text();
    }

    saveCurProfile1();
}


bool RotProfile::operator==(const RotProfile &profile)
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

bool RotProfile::operator!=(const RotProfile &profile)
{
    return !operator==(profile);
}
