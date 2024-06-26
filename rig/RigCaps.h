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
            bool canSendMorse = false,
            bool isNetworkOnly = false,
            bool needPolling = false,
            bool canProcessDXSpot = false,
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
    bool canSendMorse;
    int serialDataBits;
    int serialStopBits;
    bool isNetworkOnly;
    bool needPolling;
    bool canProcessDXSpot;
};

#endif // RIG_RIGCAPS_H
