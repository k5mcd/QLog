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

    int savePassword(const QString &storage_key, QString user, const QString &pass);
    QString getPassword(const QString &storage_key, QString user);
    void deletePassword(const QString &storage_key, QString user);

};

#endif // CREDENTIALSTORE_H
