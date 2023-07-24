#include "Callsign.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.core.callsign");

Callsign::Callsign(const QString &callsign,
                   QObject *parent)
    : QObject{parent},
      fullCallsign(callsign.toUpper()),
      valid(false)
{
    FCT_IDENTIFICATION;

    QRegularExpression callsignRE = callsignRegEx();
    QRegularExpressionMatch match = callsignRE.match(callsign);

    if ( match.hasMatch() )
    {
        //it is a valid callsign
        valid = true;
        hostPrefixWithDelimiter = match.captured(1);
        hostPrefix              = match.captured(2);
        base                    = match.captured(3);
        basePrefix              = match.captured(4);
        suffixWithDelimiter     = match.captured(7);
        suffix                  = match.captured(8);

        qCDebug(runtime) << hostPrefix << base << suffix;
    }
    else
    {
        //it is an invalid callsign
        fullCallsign = QString();
    }
}

const QRegularExpression Callsign::callsignRegEx()
{
    FCT_IDENTIFICATION;
    return QRegularExpression(callsignRegExString(), QRegularExpression::CaseInsensitiveOption);
}

const QString Callsign::callsignRegExString()
{
    FCT_IDENTIFICATION;
    return QString("^(([A-Z0-9]+)[\\/])?(([A-Z][0-9]|[A-Z]{1,2}|[0-9][A-Z])([0-9]|[0-9]+)([A-Z]+))([\\/]([A-Z0-9]+))?");
}

QString Callsign::getCallsign() const
{
    FCT_IDENTIFICATION;

    return fullCallsign;
}

QString Callsign::getHostPrefix() const
{
    FCT_IDENTIFICATION;

    return hostPrefix;
}

QString Callsign::getHostPrefixWithDelimiter() const
{
    FCT_IDENTIFICATION;

    return hostPrefixWithDelimiter;
}

QString Callsign::getBase() const
{
    FCT_IDENTIFICATION;

    return base;
}

QString Callsign::getBasePrefix() const
{
    FCT_IDENTIFICATION;

    return basePrefix;
}

QString Callsign::getSuffix() const
{
    FCT_IDENTIFICATION;

    return suffix;
}

QString Callsign::getSuffixWithDelimiter() const
{
    FCT_IDENTIFICATION;

    return suffixWithDelimiter;
}

bool Callsign::isValid() const
{
    FCT_IDENTIFICATION;

    return valid;
}

// Based on wiki information
// https://en.wikipedia.org/wiki/Amateur_radio_call_signs
const QStringList Callsign::secondarySpecialSuffixes =
{
    "A",   // operator at a secondary location registered with the licensing authorities
    "AM",  // aeronautical mobile
    "M",   // mobile operation
    "MM",  // marine mobile
    "P",   // portable operation
    "QRP",  // QRP - unofficial
    "R",    // repeaters
    "B"     // beacon
};
