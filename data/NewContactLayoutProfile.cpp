#include <QSqlQuery>
#include <QSqlError>

#include "NewContactLayoutProfile.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.data.newcontactlayoutprofile");

QDataStream& operator<<(QDataStream& out, const NewContactLayoutProfile& v)
{
    out << v.profileName
        << v.rowA
        << v.rowB;

    return out;
}

QDataStream& operator>>(QDataStream& in, NewContactLayoutProfile& v)
{
    in >> v.profileName;
    in >> v.rowA;
    in >> v.rowB;

    return in;
}

NewContactLayoutProfilesManager::NewContactLayoutProfilesManager(QObject *parent) :
    QObject(parent),
    ProfileManager<NewContactLayoutProfile>("newcontact/layoutprofile")
{
    FCT_IDENTIFICATION;

    QSqlQuery profileQuery;

    if ( ! profileQuery.prepare("SELECT profile_name, row_A, row_B FROM newcontact_layout_profiles") )
    {
        qWarning()<< "Cannot prepare select";
    }

    if ( profileQuery.exec() )
    {
        while (profileQuery.next())
        {
            NewContactLayoutProfile profileDB;
            profileDB.profileName = profileQuery.value(0).toString();
            profileDB.rowA = toIntList(profileQuery.value(1).toString());
            profileDB.rowB = toIntList(profileQuery.value(2).toString());
            addProfile(profileDB.profileName, profileDB);
        }
    }
    else
    {
        qInfo() << "NewContactLayout Profile DB select error " << profileQuery.lastError().text();
    }
}

NewContactLayoutProfilesManager *NewContactLayoutProfilesManager::instance()
{
    FCT_IDENTIFICATION;

    static NewContactLayoutProfilesManager instance;
    return &instance;
}

void NewContactLayoutProfilesManager::save()
{
    FCT_IDENTIFICATION;

    QSqlQuery deleteQuery;
    QSqlQuery insertQuery;

    if ( ! deleteQuery.prepare("DELETE FROM newcontact_layout_profiles") )
    {
        qWarning() << "Cannot prepare Delete statement";
        return;
    }

    if ( ! insertQuery.prepare("INSERT INTO newcontact_layout_profiles(profile_name, row_A, row_B) "
                               "VALUES (:profile_name, :row_A, :row_B)") )
    {
        qWarning() << "Cannot prepare Insert statement";
        return;
    }

    if ( deleteQuery.exec() )
    {
        auto keys = profileNameList();
        for ( auto &key: qAsConst(keys) )
        {
            NewContactLayoutProfile layoutProfile = getProfile(key);

            insertQuery.bindValue(":profile_name", key);
            insertQuery.bindValue(":row_A", toDBStringList(layoutProfile.rowA));
            insertQuery.bindValue(":row_B", toDBStringList(layoutProfile.rowB));

            if ( ! insertQuery.exec() )
            {
                qInfo() << "NewContactLayoutProfile DB insert error " << insertQuery.lastError().text() << insertQuery.lastQuery();
            }
        }
    }
    else
    {
        qInfo() << "NewContactLayoutProfile Profile DB delete error " << deleteQuery.lastError().text();
    }

    saveCurProfile1();
}

QString NewContactLayoutProfilesManager::toDBStringList(const QList<int> &list) const
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << list;

    QStringList stringsList;
    for ( const int item : list)
    {
        stringsList << QString::number(item);
    }
    qCDebug(runtime) << "return:" << stringsList;
    return stringsList.join(",");
}

QList<int> NewContactLayoutProfilesManager::toIntList(const QString &list) const
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << list;

    QList<int> retList;

    if ( list.isEmpty() )
        return retList;

    const QStringList splitList = list.split(",");
    for ( const QString &item : splitList )
    {
        retList << item.toInt();
    }

    qCDebug(runtime) << "return:" << retList;
    return retList;
}

bool NewContactLayoutProfile::operator==(const NewContactLayoutProfile &profile)
{
    return (profile.profileName == this->profileName
            && profile.rowA == this->rowA
            && profile.rowB == this->rowB);
}

bool NewContactLayoutProfile::operator!=(const NewContactLayoutProfile &profile)
{
    return !operator==(profile);
}
