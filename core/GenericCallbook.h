#ifndef GENERICCALLBOOK_H
#define GENERICCALLBOOK_H

#include <QObject>
#include <QNetworkReply>

class GenericCallbook : public QObject
{
    Q_OBJECT
public:
    explicit GenericCallbook(QObject *parent = nullptr);
    ~GenericCallbook() {};
    const static QString SECURE_STORAGE_KEY;
    const static QString CONFIG_USERNAME_KEY;
    const static QString CONFIG_SELECTED_CALLBOOK_KEY;

signals:
    void callsignResult(const QMap<QString, QString>& data);
    void lookupError(const QString);
    void loginFailed();

public slots:
    virtual void queryCallsign(QString callsign) {};
protected slots:
//    virtual void processReply(QNetworkReply* reply) {};

};
#endif // GENERICCALLBOOK_H
