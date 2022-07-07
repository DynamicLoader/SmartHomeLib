#ifndef __SMARTHOMELIB_H__
#define __SMARTHOMELIB_H__

// Smart Arduino Things Library
/*
        =========== User config begin ===========

*/
    #define ENABLE_ARDUINO_STRING 1
    #define HLIST_SIZE 20
    #define MSG_BUFFER_SIZE 512
/*
        =========== User config end   ===========
*/

//Pre checks
#if !defined(__cplusplus) || (__cplusplus < 201103L)
#error This library required C++11 and higher,please check your compiler and args!
#endif

#if defined(ARDUINO) && defined(ENABLE_ARDUINO_STRING)
#define USE_ARDUINO_STRING
#endif

#ifdef USE_ARDUINO_STRING
#include <Arduino.h>
namespace SmartHome {
typedef String HString;
#define HString_strlen(obj) obj.length()
#define HString_find(obj, str, from) obj.indexOf(str, from)
#define HString_substr(obj, from, to) obj.substring(from, to)
#define HString_comp(x,y) x.compareTo(y)
}
#else
#include <string>
namespace SmartHome {
typedef std::string HString;
#define HString_strlen(obj) obj.length()
#define HString_find(obj, str, from) obj.find(str, from)
#define HString_substr(obj, from, to) obj.substr(from, to)
#define HString_comp(x,y) x.compare(y)
}
#endif // ARDUINO

#include <stdint.h>

extern "C" {
#include "helper/cwpack.h"
#include "helper/cwpack_impl.h"
#include "helper/cwpack_utils.h"
}

#include "SClient.h"
#include "SUtils.hpp"
#include "SProtocol.hpp"

namespace SmartHome {
class Node;
class Property;

class Device {
public:
    Device(Adapter* client, const char* baseTopic)
        : _cli(client)
        , _baseTopic(baseTopic)
    {
    }

    Device(Adapter* client, const char* baseTopic, const char* deviceID, const char* deviceName, const char* deviceFirmware)
        : _cli(client)
        , _baseTopic(baseTopic)
        , _id(deviceID)
        , _name(deviceName)
        , _firmware(deviceFirmware)
    {
    }

    void setBaseTopic(const char* baseTopic) { this->_baseTopic = baseTopic; }
    void setDeviceInfo(const char* deviceID, const char* deviceName, const char* deviceFirmware)
    {
        this->_id = deviceID;
        this->_name = deviceName;
        this->_firmware = deviceFirmware;
    }

    bool begin(); // Begin connection only;return true if success.
    bool end();
    bool changing();
    bool ready(); // Call this when all nodes and properties are setted.

    bool addNode(Node* node) { return this->_nodeList.insert(node); }
    bool removeNode(Node* node) { return this->_nodeList.remove(node); }

    void loop();

private:
    Adapter* _cli;
    HString _baseTopic = "";
    HString _id = "";
    HString _name = "";
    HString _firmware = "";

    typedef enum {
        is_begin = 0,
        is_ready
    } _flags_definations;

    internal::HBits<uint8_t> _flags;
    SmartHome::internal::HList<Node*> _nodeList;

    // void _processDeviceBroadcast(const char* topic, const uint8_t* msg, size_t len, size_t pos);
    internal::Msgpack<MSG_BUFFER_SIZE> _makeDeviceMsg(internal::PDeviceState_t status);

protected:
    bool _publishDeviceAttr(const char* attr, const uint8_t* msg,size_t len) { return this->_cli->publish((this->_baseTopic + '/' + this->_id + '/' + attr).c_str(), msg, len, true); }
    bool _publishDeviceAttr(const char* attr, const char* msg) { return this->_cli->publish((this->_baseTopic + '/' + this->_id + '/' + attr).c_str(), (const uint8_t*)msg, sizeof(msg), true); }
    bool _publishDeviceAttr(const char* attr, internal::Msgpack<MSG_BUFFER_SIZE>& msg) { return this->_cli->publish((this->_baseTopic + '/' + this->_id + '/' + attr).c_str(),msg.data(), msg.length(), true); }

    friend class Node;
    friend class Property;
    
};

class Node {
public:
    Node(const char* nodeID, const char* nodeName, const char* nodeType)
        : _id(nodeID)
        , _name(nodeName)
        , _type(nodeType)
    {
    }

    void setNodeInfo(const char* nodeID, const char* nodeName, const char* nodeType)
    {
        this->_id = nodeID;
        this->_name = nodeName;
        this->_type = nodeType;
    }
    bool addProp(Property* property) { return this->_propList.insert(property); }
    bool removeProp(Property* property) { return this->_propList.remove(property); }

private:
    HString _id = "";
    HString _name = "";
    HString _type = "";
    Device* _parent = nullptr;

    internal::HList<Property*> _propList;

    bool _publishNode();
    bool _publishNodeAttr(const char* attr, const char* msg) { return this->_parent->_publishDeviceAttr((this->_id + '/' + attr).c_str(), msg); }
    bool _publishNodeAttr(const char* attr, const uint8_t* msg,size_t len) { return this->_parent->_publishDeviceAttr((this->_id + '/' + attr).c_str(), msg,len); }
    bool _publishNodeAttr(const char* attr, internal::Msgpack<MSG_BUFFER_SIZE>& msg) { return this->_parent->_publishDeviceAttr((this->_id + '/' + attr).c_str(), msg.data(),msg.length()); }
    void _processNodeCB(internal::HStringSplitResult topic,const uint8_t* msg,size_t len);

protected:
    friend class Device;
    friend class Property;
    friend class internal::HList<Node*>;
};

class Property {
public:
    Property(const char* propID, const char* propName) { this->setPropertyInfo(propID, propName); }
    void setPropertyInfo(const char* propID, const char* propName)
    {
        this->_id = propID;
        this->_name = propName;
    }

protected:
    HString _id = "";
    HString _name = "";
    Node* _parent = nullptr;

    bool _publishPropAttr(const char* attr, const char* msg);
    bool _publishPropValue(const char* attr, const uint8_t* msg, size_t len, bool retained);

    friend class Device;
    friend class Node;
    friend class internal::HList<Property*>;
};

} // namespace SmartHome


#include "SDevice.hpp"
#include "SNode.hpp"
#include "SProperty.hpp"


#endif // End of File