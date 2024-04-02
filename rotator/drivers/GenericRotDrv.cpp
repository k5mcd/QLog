#include "GenericRotDrv.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.core.rot.driver.genericrotdrv");

GenericRotDrv::GenericRotDrv(const RotProfile &profile,
                             QObject *parent)
    : QObject{parent},
      rotProfile(profile),
      opened(false),
      azimuth(0.0),
      elevation(0.0)
{
    FCT_IDENTIFICATION;
}

const RotProfile GenericRotDrv::getCurrRotProfile() const
{
    FCT_IDENTIFICATION;

    return rotProfile;
}

const QString GenericRotDrv::lastError() const
{
    FCT_IDENTIFICATION;

    return lastErrorText;
}
