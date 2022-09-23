#ifndef CALLBOOKMANAGER_H
#define CALLBOOKMANAGER_H

#include <QObject>
#include <QPointer>
#include "core/GenericCallbook.h"

class CallbookManager : public QObject
{
    Q_OBJECT
public:
    explicit CallbookManager(QObject *parent = nullptr);

    void queryCallsign(const QString &callsign);
    bool isActive();

signals:
    void loginFailed(QString);
    void callsignResult(const QMap<QString, QString>& data);
    void callsignNotFound(QString);
    void lookupError(QString);

public slots:
    void initCallbooks();
    void abortQuery();

private slots:
    void primaryCallbookCallsignNotFound(QString);
    void secondaryCallbookCallsignNotFound(QString);
    void processCallsignResult(const QMap<QString, QString>& data);

private:
    GenericCallbook *createCallbook(const QString&);

private:
    QPointer<GenericCallbook> primaryCallbook;
    QPointer<GenericCallbook> secondaryCallbook;
    QString currentQueryCallsign;
    static QCache<QString, QMap<QString, QString>> queryCache;

};

#endif // CALLBOOKMANAGER_H
