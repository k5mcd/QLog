#include "AlertEvaluator.h"
#include "debug.h"
#include "data/DxSpot.h"
#include "data/WsjtxEntry.h"

MODULE_IDENTIFICATION("qlog.ui.alertevaluator");

AlertEvaluator::AlertEvaluator(QObject *parent)
    : QObject{parent}
{
    FCT_IDENTIFICATION;
}

void AlertEvaluator::dxSpot(const DxSpot & spot)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << "DX Spot";

    UserAlert userAlert;
    userAlert.test = spot.callsign;
    emit alert(userAlert);
}

void AlertEvaluator::WSJTXCQSpot(const WsjtxEntry &wsjtx)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << "WSJTX CQ Spot";

    UserAlert userAlert;
    userAlert.test = wsjtx.callsign;
    emit alert(userAlert);
}
