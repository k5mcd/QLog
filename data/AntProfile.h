#ifndef ANTPROFILE_H
#define ANTPROFILE_H

#include <QString>
#include <QObject>

#include "data/ProfileManager.h"


#define DEFAULT_ROT_MODEL 1

class AntProfile
{
public:
    AntProfile(){};
    QString profileName;
    QString description;

private:
    friend QDataStream& operator<<(QDataStream& out, const AntProfile& v);
    friend QDataStream& operator>>(QDataStream& in, AntProfile& v);
};

Q_DECLARE_METATYPE(AntProfile)

class AntProfilesManager : QObject, public ProfileManager<AntProfile>
{
    Q_OBJECT

public:

    explicit AntProfilesManager(QObject *parent = nullptr);
    ~AntProfilesManager() { };

    static AntProfilesManager *instance();
    void save();

};


#endif // ANTPROFILE_H
