#ifndef CWDAEMONKEY_H
#define CWDAEMONKEY_H

#include <QObject>
#include "CWKey.h"

class CWDaemonKey : public CWKey,
                    protected CWKeyUDPInterface
{
    Q_OBJECT

public:
    explicit CWDaemonKey(const QString &hostname,
                         const quint16 port,
                         const CWKey::CWKeyModeID mode,
                         const qint32 defaultSpeed,
                         QObject *parent = nullptr);
    ~CWDaemonKey(){};

    virtual bool open() override;
    virtual bool close() override;
    virtual QString lastError() override;

    virtual bool sendText(const QString &text) override;
    virtual bool setWPM(const qint16 wpm) override;
    virtual bool imediatellyStop() override;

protected:
    QString lastLogicalError;
    bool isOpen;
    const QChar ESCChar;
};

#endif // CWDAEMONKEY_H
