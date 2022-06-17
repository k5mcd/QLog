#include <QSettings>
#include "CallbookManager.h"
#include "core/debug.h"
#include "core/HamQTH.h"
#include "core/QRZ.h"

MODULE_IDENTIFICATION("qlog.ui.callbookmanager");

CallbookManager::CallbookManager(QObject *parent)
    : QObject{parent}
{
    FCT_IDENTIFICATION;

    initCallbooks();
}

void CallbookManager::queryCallsign(const QString &callsign)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << callsign;

    if ( !primaryCallbook.isNull() )
    {
        currentQueryCallsign = callsign;
        primaryCallbook->queryCallsign(currentQueryCallsign);
    }
    else
    {
        emit callsignNotFound(callsign);
    }
}

GenericCallbook *CallbookManager::createCallbook(const QString &callbookID)
{
    FCT_IDENTIFICATION;

    GenericCallbook *ret = nullptr;
    QString callbookString;

    if (callbookID == HamQTH::CALLBOOK_NAME )
    {
        ret = new HamQTH(this);
        callbookString = tr("HamQTH");
    }
    else if ( callbookID == QRZ::CALLBOOK_NAME )
    {
        ret = new QRZ(this);
        callbookString = tr("QRZ.com");
    }

    if ( ret )
    {
        connect(ret, &GenericCallbook::callsignResult, this, &CallbookManager::processCallsignResult);
        connect(ret, &GenericCallbook::loginFailed, this, [this, callbookString]()
        {
            currentQueryCallsign = QString();
            emit loginFailed(callbookString);
        });
    }

    return ret;
}

void CallbookManager::initCallbooks()
{
    QSettings settings;

    primaryCallbook.clear();
    secondaryCallbook.clear();

    QString primaryCallbookSelection = settings.value(GenericCallbook::CONFIG_PRIMARY_CALLBOOK_KEY).toString();
    QString secondaryCallbookSelection = settings.value(GenericCallbook::CONFIG_SECONDARY_CALLBOOK_KEY).toString();

    primaryCallbook = createCallbook(primaryCallbookSelection);
    secondaryCallbook = createCallbook(secondaryCallbookSelection);

    if ( !primaryCallbook.isNull()
         && !secondaryCallbook.isNull() )
    {
        connect(primaryCallbook, &GenericCallbook::callsignNotFound, this, &CallbookManager::primaryCallbookCallsignNotFound);
        connect(secondaryCallbook, &GenericCallbook::callsignNotFound, this, &CallbookManager::secondaryCallbookCallsignNotFound);
    }
}

void CallbookManager::primaryCallbookCallsignNotFound(QString notFoundCallsign)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << notFoundCallsign;

    if ( notFoundCallsign != currentQueryCallsign ) return ;

    if ( secondaryCallbook.isNull() )
    {
        emit callsignNotFound(notFoundCallsign);
        return;
    }

    qCDebug(runtime) << "Callsign not found - primary - trying the secondary callbook";

    secondaryCallbook->queryCallsign(notFoundCallsign);
}

void CallbookManager::secondaryCallbookCallsignNotFound(QString notFoundCallsign)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << notFoundCallsign;

    if ( notFoundCallsign != currentQueryCallsign ) return ;

    qCDebug(runtime) << "Callsign not found - secondary ";

    emit callsignNotFound(notFoundCallsign);
}

void CallbookManager::processCallsignResult(const QMap<QString, QString> &data)
{
    FCT_IDENTIFICATION;

    emit callsignResult(data);
}
