#ifndef CREDENTIALSTORE_H
#define CREDENTIALSTORE_H

#include <QObject>
#include <QString>

class CredentialStore : public QObject
{
    Q_OBJECT
public:
    explicit CredentialStore(QObject *parent = nullptr);
    static CredentialStore* instance();

    int savePassword(QString storage_key, QString user, QString pass);
    QString getPassword(QString storage_key, QString user);
    void deletePassword(QString storage_key, QString user);

};

#endif // CREDENTIALSTORE_H
