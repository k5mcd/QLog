#include "CWDummyKey.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.data.cwdummykey");

CWDummyKey::CWDummyKey(QObject *parent)
    : CWKey(CWKey::IAMBIC_B, 25, parent),
      isUsed(false)
{
    FCT_IDENTIFICATION;
}

bool CWDummyKey::open()
{
    FCT_IDENTIFICATION;

    qInfo() << "Key is Connected";

    isUsed = true;

    return true;
}

bool CWDummyKey::close()
{
    FCT_IDENTIFICATION;

    qInfo() << "Key is Disconnected";

    isUsed = false;

    return true;
}

bool CWDummyKey::sendText(const QString &text)
{
    FCT_IDENTIFICATION;

    if ( isUsed )
    {
        qInfo() << "Sending " << text;
    }

    return true;
}

bool CWDummyKey::setWPM(const qint16 wpm)
{
    FCT_IDENTIFICATION;

    if ( isUsed )
    {
        qInfo() << "Setting Speed " << wpm;
    }

    return true;
}

QString CWDummyKey::lastError()
{
    FCT_IDENTIFICATION;

    return QString();
}

bool CWDummyKey::imediatellyStop()
{
    FCT_IDENTIFICATION;

    if ( isUsed )
    {
        qInfo() << "imediatelly Stop";
    }

    return true;
}

