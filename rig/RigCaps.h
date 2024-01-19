#ifndef RIG_RIGCAPS_H
#define RIG_RIGCAPS_H


class RigCaps
{
public:
    RigCaps(bool canGetFreq = false,
            bool canGetMode = false,
            bool canGetVFO = false,
            bool canGetPWR = false,
            bool canGetRIT = false,
            bool canGetXIT = false,
            bool canGetPTT = false,
            bool canGetKeySpeed = false,
            bool isNetworkOnly = false,
            int serial_data_bits = 8,
            int serial_stop_bits = 1
            );

    bool canGetFreq;
    bool canGetMode;
    bool canGetVFO;
    bool canGetPWR;
    bool canGetRIT;
    bool canGetXIT;
    bool canGetPTT;
    bool canGetKeySpeed;
    int serialDataBits;
    int serialStopBits;
    bool isNetworkOnly;
};

#endif // RIG_RIGCAPS_H
