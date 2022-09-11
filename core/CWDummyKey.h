#ifndef CWDUMMYKEY_H
#define CWDUMMYKEY_H

#include "CWKey.h"

class CWDummyKey : public CWKey
{
    Q_OBJECT

public:
    explicit CWDummyKey(QObject *parent = nullptr);

    virtual bool open() override;
    virtual bool close() override;
    virtual bool sendText(const QString &text) override;
    virtual bool setWPM(const qint16 wpm) override;
    virtual QString lastError() override;
    virtual bool imediatellyStop() override;

private:
    bool isUsed;
};

#endif // CWDUMMYKEY_H
