#ifndef BANDPLAN_H
#define BANDPLAN_H

#include <QtCore>

class BandPlan
{
public:

    static const QString MODE_GROUP_STRING_CW;
    static const QString MODE_GROUP_STRING_DIGITAL;
    static const QString MODE_GROUP_STRING_FT8;
    static const QString MODE_GROUP_STRING_PHONE;

    enum BandPlanModes
    {
        BAND_MODE_CW,
        BAND_MODE_DIGITAL,
        BAND_MODE_FT8,
        BAND_MODE_LSB,
        BAND_MODE_USB,
        BAND_MODE_PHONE
    };

    static BandPlanModes freq2BandMode(const double freq);
    static QString freq2BandModeGroupString(const double freq);

    BandPlan();
};

#endif // BANDPLAN_H
