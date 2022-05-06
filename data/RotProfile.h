#ifndef ROTPROFILE_H
#define ROTPROFILE_H

#include <QString>
#include <QObject>
#include <QDataStream>
#include <QVariant>

#include "data/ProfileManager.h"

#define DEFAULT_ROT_MODEL 1

class RotProfile
{
public:
    RotProfile() { model = DEFAULT_ROT_MODEL; netport = 0;
                   baudrate = 0; databits = 0; stopbits = 0.0;};

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

    bool operator== (const RotProfile &profile);
    bool operator!= (const RotProfile &profile);

private:
    friend QDataStream& operator<<(QDataStream& out, const RotProfile& v);
    friend QDataStream& operator>>(QDataStream& in, RotProfile& v);

};

Q_DECLARE_METATYPE(RotProfile);

class RotProfilesManager : QObject, public ProfileManager<RotProfile>
{
    Q_OBJECT

public:

    explicit RotProfilesManager(QObject *parent = nullptr);
    ~RotProfilesManager() { };

    static RotProfilesManager* instance();
    void save();

};

#endif // ROTPROFILE_H
