#include "GenericRigDrv.h"
#include "core/debug.h"


MODULE_IDENTIFICATION("qlog.core.rig.driver.genericrigdrv");

GenericRigDrv::GenericRigDrv(const RigProfile &profile, QObject *parent)
    : QObject{parent},
      rigProfile(profile),
      opened(false)
{
    FCT_IDENTIFICATION;

}

const RigProfile GenericRigDrv::getCurrRigProfile() const
{
    FCT_IDENTIFICATION;

    return rigProfile;
}

const QString GenericRigDrv::lastError() const
{
    FCT_IDENTIFICATION;

    return lastErrorText;
}
