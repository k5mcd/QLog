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
    RigProfile() {
                   model = DEFAULT_RIG_MODEL; netport = 0; baudrate = 0;
                   databits = 0; stopbits = 0.0; pollInterval = 0;
                   txFreqStart = 0.0; txFreqEnd = 0.0; getFreqInfo = false;
                   getModeInfo = false; getVFOInfo = false; getPWRInfo = false;
                   ritOffset = 0.0; xitOffset = 0.0, getRITInfo = false;
                   getXITInfo = true; defaultPWR = 0.0;
                 };

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
    quint32 pollInterval;
    double ritOffset;
    double xitOffset;
    float txFreqStart;
    float txFreqEnd;
    bool getFreqInfo;
    bool getModeInfo;
    bool getVFOInfo;
    bool getPWRInfo;
    bool getRITInfo;
    bool getXITInfo;
    double defaultPWR;


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
