#ifndef LOGPARAM_H
#define LOGPARAM_H

#include <QObject>

class LogParam : public QObject
{
    Q_OBJECT
public:
    explicit LogParam(QObject *parent = nullptr);

    static bool setParam(const QString&, const QString&);
    static QString getParam(const QString&);
};

#endif // LOGPARAM_H
