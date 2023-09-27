#ifndef NEWCONTACTLAYOUTPROFILE_H
#define NEWCONTACTLAYOUTPROFILE_H

#include <QString>
#include <QObject>
#include <QDataStream>

#include "data/ProfileManager.h"

class NewContactLayoutProfile
{
public:
    NewContactLayoutProfile(){};

    QString profileName;
    QList<int> rowA;
    QList<int> rowB;
    QList<int> detailColA;
    QList<int> detailColB;
    QList<int> detailColC;

    bool operator== (const NewContactLayoutProfile &profile);
    bool operator!= (const NewContactLayoutProfile &profile);
    static NewContactLayoutProfile getClassicLayout();

private:
    friend QDataStream& operator<<(QDataStream& out, const NewContactLayoutProfile& v);
    friend QDataStream& operator>>(QDataStream& in, NewContactLayoutProfile& v);

};

Q_DECLARE_METATYPE(NewContactLayoutProfile);

class NewContactLayoutProfilesManager : QObject, public ProfileManager<NewContactLayoutProfile>
{
    Q_OBJECT

public:

    explicit NewContactLayoutProfilesManager(QObject *parent = nullptr);
    ~NewContactLayoutProfilesManager() { };

    static NewContactLayoutProfilesManager* instance();
    void save();

    QString toDBStringList(const QList<int> &list) const;
    QList<int> toIntList(const QString &list) const;
};


#endif // NEWCONTACTLAYOUTPROFILE_H
