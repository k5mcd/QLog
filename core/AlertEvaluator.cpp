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

    userAlert.dateTime = QDateTime::currentDateTimeUtc();
    userAlert.source = UserAlert::ALERTSOURCETYPE::DXSPOT;
    userAlert.triggerName = "test rule DX";
    userAlert.callsign = spot.callsign;
    userAlert.freq = spot.freq;
    userAlert.mode = spot.mode;
    userAlert.dxcc = spot.dxcc;
    userAlert.status = spot.status;
    userAlert.comment = spot.comment;

    emit alert(userAlert);
}

void AlertEvaluator::WSJTXCQSpot(const WsjtxEntry &wsjtx)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << "WSJTX CQ Spot";

    UserAlert userAlert;

    userAlert.dateTime = QDateTime::currentDateTimeUtc();
    userAlert.source = UserAlert::ALERTSOURCETYPE::DXSPOT;
    userAlert.triggerName = "test rule WSJTX";
    userAlert.callsign = wsjtx.callsign;
    userAlert.freq = wsjtx.freq;
    userAlert.mode = Data::freqToMode(wsjtx.freq);
    userAlert.dxcc = wsjtx.dxcc;
    userAlert.status = wsjtx.status;
    userAlert.comment = wsjtx.decode.message;

    emit alert(userAlert);
}
