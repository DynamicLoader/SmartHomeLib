#ifndef __SMARTHOME_PROTOCOL_HPP__
#define __SMARTHOME_PROTOCOL_HPP__

#include "SmartHomeLib.h"

// Predefinations
#define PROTOCOL_VERSION 0

namespace SmartHome {
namespace internal{

    typedef struct {
        bool success=false;
        uint8_t data[256]={0};
        size_t len=0;
    } PMsgPack_t;

    typedef enum {
        Init = 0,
        Online,
        Sleep,
        Lost
    } PDeviceState_t;

    
}
} // End of namespace SmartHome

#endif

// End of file SProtocol.hpp