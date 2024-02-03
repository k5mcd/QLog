#include "RigCaps.h"

RigCaps::RigCaps(bool canGetFreq,
                 bool canGetMode,
                 bool canGetVFO,
                 bool canGetPWR,
                 bool canGetRIT,
                 bool canGetXIT,
                 bool canGetPTT,
                 bool canGetKeySpeed,
                 bool isNetworkOnly,
                 bool needPolling,
                 int serial_data_bits,
                 int serial_stop_bits
                 ) :
    canGetFreq(canGetFreq),
    canGetMode(canGetMode),
    canGetVFO(canGetVFO),
    canGetPWR(canGetPWR),
    canGetRIT(canGetRIT),
    canGetXIT(canGetXIT),
    canGetPTT(canGetPTT),
    canGetKeySpeed(canGetKeySpeed),
    serialDataBits(serial_data_bits),
    serialStopBits(serial_stop_bits),
    isNetworkOnly(isNetworkOnly),
    needPolling(needPolling)
{

}
