#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include "SmartHomeLib.h"

namespace SmartHome {

bool Device::begin()
{
    using namespace internal;
    PMsgPack_t msg;

    // We first prepare the will msg.
    msg = makeDeviceMsg(Lost, this);
    bool ret = this->_cli->connect((this->_baseTopic + '/' + this->_id).c_str(), msg.data, msg.len);
    if (!ret)
        return false;

    // We next prepare the msgpack of Device level

    return ret;
}

bool Device::end()
{
    this->_publishDeviceAttr("$state", "disconnected");
    this->_flags.set(is_ready, false);
    bool ret = this->_cli->disconnect();
    return ret;
}

bool Device::changing()
{
    if (this->_publishDeviceAttr("$state", "init")) {
        this->_flags.set(is_ready, false);
        return true;
    }
    return false;
}

bool Device::ready()
{
    if (this->_flags.get(is_ready))
        return false;
    HString nodes;
    bool ret = true;

    if (this->_nodeList.count() == 0)
        return false; // No node
    this->_nodeList.sort();
    for (uint8_t i = 0; i < this->_nodeList.count(); i++) {
        nodes += "," + this->_nodeList.raw()[i]->_id;
    }

    ret &= this->_publishDeviceAttr("$nodes", (const uint8_t*)(nodes.c_str() + 1)); // ignore the first ','
    ret &= this->_publishDeviceAttr("$state", "ready");
    if (ret)
        this->_flags.set(is_ready, true);
    return ret;
}

void Device::loop()
{
    if (!this->_flags.get(is_ready))
        return;
    char* topic = nullptr;
    uint8_t* msg = nullptr;
    size_t len = 0;
    if (!(this->_cli->getMsg(topic, msg, len)))
        return;
    if ((!topic) || (!msg) || (len == 0))
        return;

    //  === Process msg ===

    HString origin = topic;
    HString tmp;
    size_t pos = 0;

    if (!HStringDlm(origin, tmp, pos) || tmp != this->_baseTopic) // Find the 1st section and compare with base topic
        return;

    if (!HStringDlm(origin, tmp, pos) || tmp != this->_id) // Find 2nd section and compare with device id
        return;

    if (!HStringDlm(origin, tmp, pos)) // Get 3nd section
        return;
    if (tmp == "$broadcast")
        this->_processDeviceBroadcast(topic, msg, len, pos);

    // Is Node attribute
    (this->_nodeList.find(tmp))->_processNodeCB(origin, msg, len, pos);
}

void Device::_processDeviceBroadcast(const char* topic, const uint8_t* msg, size_t len, size_t pos)
{
    // HString origin=topic;
    // HString tmp="";
    // size_t stmp=0;
    // size_t slen = origin.length();
}

// Protocol Helper of Device

internal::PMsgPack_t Device::makeDeviceMsg(internal::PDeviceState_t status, SmartHome::Device* dev)
{
    using namespace internal;
    PMsgPack_t ret;
    cw_pack_context ctx;
    cw_pack_context_init(&ctx, (void*)ret.data, sizeof(ret.data), nullptr);

    switch (status) {

    case Online:
        cw_pack_unsigned(&ctx, 0);
        cw_pack_str(&ctx, dev->_name.c_str(), dev->_name.length()); // 2nd.Device Name
        cw_pack_str(&ctx, dev->_firmware.c_str(), dev->_firmware.length()); // 3rd.Device Firmware
    case Lost:
        cw_pack_unsigned(&ctx, PROTOCOL_VERSION); // 0.Protocol version
        cw_pack_signed(&ctx, Lost); // 1st. Pack type
        ret.success = true;
        ret.len = ctx.current - ctx.start;
        return ret;

    default:
        ret.success=false;
        return ret;
    }
}

} // namespace SmartHome

#endif