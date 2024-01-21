#ifndef QLOG_CORE_LOGPARAM_H
#define QLOG_CORE_LOGPARAM_H

#include <QObject>

class LogParam : public QObject
{
    Q_OBJECT
public:
    explicit LogParam(QObject *parent = nullptr);

    static bool setParam(const QString&, const QString&);
    static QString getParam(const QString&);

private:
    static QCache<QString, QString> localCache;
};

#endif // QLOG_CORE_LOGPARAM_H
