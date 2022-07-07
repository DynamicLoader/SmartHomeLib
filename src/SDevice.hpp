#ifndef __SMARTHOME_DEVICE_DEFS__
#define __SMARTHOME_DEVICE_DEFS__

#include "SmartHomeLib.h"

namespace SmartHome {

bool Device::begin()
{
    using namespace internal;
    Msgpack<MSG_BUFFER_SIZE> msg;

    // We first prepare the will msg.
    msg = this->_makeDeviceMsg(Lost);
    bool ret = this->_cli->connect((this->_baseTopic + '/' + this->_id).c_str(), msg.data(), msg.length());
    if (!ret)
        return false;

    // We next prepare the msgpack of Device level
    msg = this->_makeDeviceMsg(Init);
    if (msg.success())
        ret = this->_publishDeviceAttr((this->_baseTopic + '/' + this->_id).c_str(), msg);

    this->_flags.set(is_begin, ret);
    return ret;
}

bool Device::end()
{
    using namespace internal;
    if (!this->_flags.get(is_begin))
        return false;
    Msgpack<MSG_BUFFER_SIZE> msg = this->_makeDeviceMsg(Offline);
    if (msg.success())
        this->_publishDeviceAttr((this->_baseTopic + '/' + this->_id).c_str(),msg);
    this->_cli->disconnect();

    return true;
}

bool Device::changing()
{
    using namespace internal;
    Msgpack<MSG_BUFFER_SIZE> msg = this->_makeDeviceMsg(Init);
    bool ret = msg.success();
    if (ret)
        ret = this->_publishDeviceAttr((this->_baseTopic + '/' + this->_id).c_str(), msg);

    this->_flags.set(is_ready, false);
    return ret;
}

bool Device::ready()
{
    using namespace internal;
    if (this->_flags.get(is_ready) || !this->_flags.get(is_begin))
        return false;
    bool ret=false;
    for (auto nptr : this->_nodeList)
        ret &= nptr->_publishNode(); // Init Node msg
    if (!ret)
        return false;
    Msgpack<MSG_BUFFER_SIZE> msg = this->_makeDeviceMsg(Ready);
    if (msg.success())
        ret = this->_publishDeviceAttr((this->_baseTopic + '/' + this->_id).c_str(), msg);
    this->_flags.set(is_ready, ret);
    return ret;
}

void Device::loop()
{
    using namespace internal;
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
    HString tp = HString(topic);
    size_t pos = HString_find(tp, this->_baseTopic + '/' + this->_id, 0);
    if (pos != 0) // Not the same base topic and device ID
        return;
    pos = this->_baseTopic.length() + this->_id.length() + 1;
    if (pos == tp.length()) // Device description level,ignored.
        return;
    auto ret = HStringSplit(topic, pos, '/');
    if (ret.count < 2) // No sub level found.
        return;
    HString nodeid = HString_substr(tp, pos + ret.offsets[0], pos + ret.offsets[1]);

    Node* n = this->_nodeList.find(nodeid);
    if (n == nullptr)
        return;
    //ToDo
}

internal::Msgpack<MSG_BUFFER_SIZE> Device::_makeDeviceMsg(internal::PDeviceState_t status)
{
    using namespace internal;
    Msgpack<MSG_BUFFER_SIZE> ret;
    ret.pack(PROTOCOL_IDENTIFIER);
    ret.pack(PROTOCOL_VERSION); // 0.Protocol version
    ret.pack(DeviceDescription); // Message type

    switch (status) {

    case Ready:
        ret.map(5); // define map size
        ret.pack("s").pack(Ready); // 1.status
        ret.pack("dN").pack(this->_name.c_str());// 2.device Name
        ret.pack("dF").pack(this->_firmware.c_str());// 3.Device Firmware

        // 4.Process Node List
        ret.pack("nL");
        ret.array(this->_nodeList.count());
        for (auto nptr : this->_nodeList)
            ret.pack(nptr->_id.c_str());

        //5. Process Plugin List (To Do)
        ret.pack("pL");
        ret.pack("dummy");

        break;
    case Lost:
        ret.map(1).pack("s").pack(Lost);
        break;
    case Sleep:
        ret.map(1).pack("s").pack(Sleep);
        break;
    case Init:
        ret.map(1).pack("s").pack(Init);
        break;
    case Offline:
        ret.map(1).pack("s").pack(Offline);
        break;
    default:
        // Do nothing
    }
    return ret;
}

} // namespace SmartHome

#endif