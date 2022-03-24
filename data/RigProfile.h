#ifndef RIGPROFILE_H
#define RIGPROFILE_H

#include <QString>
#include <QObject>
#include <QMap>
#include <QDataStream>
#include <QVariant>

#include "data/ProfileManager.h"

#define DEFAULT_RIG_MODEL 1

class RigProfile
{
public:
    RigProfile() { model = 0; netport = 0; baudrate = 0; databits = 0; stopbits = 0.0;};

    QString profileName;
    qint32 model;
    QString portPath;
    QString hostname;
    quint16 netport;
    quint32 baudrate;
    quint8 databits;
    float stopbits;
    QString flowcontrol;
    QString parity;

    bool operator== (const RigProfile &profile);
    bool operator!= (const RigProfile &profile);

private:
    friend QDataStream& operator<<(QDataStream& out, const RigProfile& v);
    friend QDataStream& operator>>(QDataStream& in, RigProfile& v);
};

Q_DECLARE_METATYPE(RigProfile)

class RigProfilesManager : QObject, public ProfileManager<RigProfile>
{
    Q_OBJECT

public:

    explicit RigProfilesManager(QObject *parent = nullptr);
    ~RigProfilesManager() { };

    static RigProfilesManager* instance();
    void save();

};


#endif // RIGPROFILE_H
