#include "RotCaps.h"

RotCaps::RotCaps(bool isNetworkOnly,
                 int serial_data_bits,
                 int serial_stop_bits) :
    isNetworkOnly(isNetworkOnly),
    serialDataBits(serial_data_bits),
    serialStopBits(serial_stop_bits)
{

}
