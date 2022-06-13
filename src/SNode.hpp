#ifndef __SMARTHOME_NODE_HPP__
#define __SMARTHOME_NODE_HPP__

#include "SmartHomeLib.h"

namespace SmartHome {

bool Node::_publishNode()
{
    if (!this->_parent)
        return false;
    bool ret = true;
    HString props;
    //if (this->_propList.begin() == this->_propList.end())
    //    return false; //No properties
    //for (auto it = this->_propList.begin(); it != this->_propList.end(); it++) {
    //    props += ',' + (*it)->_id;
    //}

    ret &= this->_publishNodeAttr("$name", this->_name.c_str());
    ret &= this->_publishNodeAttr("$type", this->_type.c_str());
    ret &= this->_publishNodeAttr("$properties", props.c_str() + 1); // Ignore the first ','
    return ret;
}

void Node::_processNodeCB(HString& topic, const uint8_t* msg, size_t len, size_t pos)
{
}

}

#endif