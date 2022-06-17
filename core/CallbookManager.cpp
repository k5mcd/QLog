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

bool CallbookManager::isActive()
{
    FCT_IDENTIFICATION;

    bool ret = ! primaryCallbook.isNull() || !secondaryCallbook.isNull();
    qCDebug(runtime) << ret;
    return ret;
}

GenericCallbook *CallbookManager::createCallbook(const QString &callbookID)
{
    FCT_IDENTIFICATION;

    GenericCallbook *ret = nullptr;

    if (callbookID == HamQTH::CALLBOOK_NAME )
    {
        ret = new HamQTH(this);
    }
    else if ( callbookID == QRZ::CALLBOOK_NAME )
    {
        ret = new QRZ(this);
    }

    if ( ret )
    {
        connect(ret, &GenericCallbook::callsignResult, this, &CallbookManager::processCallsignResult);
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

    if ( !primaryCallbook.isNull() )
    {
        connect(primaryCallbook, &GenericCallbook::callsignNotFound, this, &CallbookManager::primaryCallbookCallsignNotFound);
        connect(primaryCallbook, &GenericCallbook::loginFailed, this, [this]()
        {
            emit loginFailed(primaryCallbook->getDisplayName());
        });
        connect(primaryCallbook, &GenericCallbook::lookupError, this, [this](QString error)
        {
            emit lookupError(primaryCallbook->getDisplayName()
                             + " - " + error
                             + ((!secondaryCallbook.isNull()) ? tr("<p>The secondary callbook will be used</p>") : ""));
            primaryCallbookCallsignNotFound(currentQueryCallsign);
        });
    }

    if ( !secondaryCallbook.isNull() )
    {
        connect(secondaryCallbook, &GenericCallbook::callsignNotFound, this, &CallbookManager::secondaryCallbookCallsignNotFound);
        connect(secondaryCallbook, &GenericCallbook::loginFailed, this, [this]()
        {
            emit loginFailed(secondaryCallbook->getDisplayName());
        });
        connect(secondaryCallbook, &GenericCallbook::lookupError, this, [this](QString error)
        {
            emit lookupError(secondaryCallbook->getDisplayName() + " - " + error);
            secondaryCallbookCallsignNotFound(currentQueryCallsign);
        });
    }
}

void CallbookManager::abortQuery()
{
    FCT_IDENTIFICATION;

    if ( ! primaryCallbook.isNull() ) primaryCallbook->abortQuery();
    if ( ! secondaryCallbook.isNull() ) secondaryCallbook->abortQuery();
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
