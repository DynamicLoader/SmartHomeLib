#ifndef __SMARTHOME_PROPERTY_HPP__
#define __SMARTHOME_PROPERTY_HPP__

#include "SmartHomeLib.h"

namespace SmartHome {

bool Property::_publishPropAttr(const char* attr, const char* msg)
{
    if (this->_parent == nullptr)
        return false;
    return this->_parent->_publishNodeAttr((this->_id + '/' + attr).c_str(), msg);
}
bool Property::_publishPropValue(const char* attr, const uint8_t* msg, size_t len, bool retained)
{
    if (this->_parent == nullptr)
        return false; //SmartHomeNode
    if (this->_parent->_parent == nullptr)
        return false; //SmartHomeDevice
    if (this->_parent->_parent->_cli == nullptr)
        return false; //SmartHomeClient

    return this->_parent->_parent->_cli->publish(
        (this->_parent->_parent->_baseTopic + '/' + this->_parent->_parent->_id + '/' + this->_parent->_id + '/' + this->_id + '/' + attr).c_str(),
        (uint8_t*)(msg), len, retained);
}

}

#endif