#ifndef __SMARTHOME_PROTOCOL_CPP__
#define __SMARTHOME_PROTOCOL_CPP__

/* 
===========   Protocol definations   ===========

    ## The msgpack should follow the following format:
    
      #       Name      
      0    Protocol Identifier
      1    Protocol Rev
      2    Message type
      3    Meaasge content (Msgpack.map)
    

=================================================
*/

// Predefinations
#define PROTOCOL_VERSION 1
#define PROTOCOL_IDENTIFIER "SMP"

namespace SmartHome {
class Device;
class Node;
namespace internal {

    typedef enum {
        DeviceDescription=1,
        NodeDescription,
        PropertyDescription,
        PropertyValue,
        Broadcast,
    } PMessageType_t;

    typedef enum {
        Init = 1,
        Ready,
        Sleep,
        Offline,
        Lost,
    } PDeviceState_t;
} // End of namespace internal
} // End of namespace SmartHome

#endif
 // End of file SProtocol.hpp