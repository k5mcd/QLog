#include "GenericDrv.h"
#include "core/debug.h"


MODULE_IDENTIFICATION("qlog.core.rig.driver.genericdrv");

GenericDrv::GenericDrv(const RigProfile &profile, QObject *parent)
    : QObject{parent},
      rigProfile(profile),
      opened(false)
{
    FCT_IDENTIFICATION;

}

const RigProfile GenericDrv::getCurrRigProfile() const
{
    FCT_IDENTIFICATION;

    return rigProfile;
}

const QString GenericDrv::lastError() const
{
    FCT_IDENTIFICATION;

    return lastErrorText;
}
