#include <iostream>
#include "../SmartHomeLib.h"
#include "../SClient.h"

using namespace SmartHome;
using namespace std;

class ConsoleAdapter : public Adapter {
public:
    ConsoleAdapter(){}
    ~ConsoleAdapter(){}
    ConsoleAdapter(const char* host, uint16_t port, const char* user, const char* pwd)
    {
        cout << "[InitAdapter] Connect to " << host << ":" << port << " with user:" << user << " and "
             << "pwd:" << pwd << endl;
    }

    bool connect(const char* willTopic, const uint8_t* willMsg, size_t len)
    {
        cout << "[Connection options] willTopic:" << willTopic << " willMsg:" << willMsg << " (len:" << len << ")" << endl;
        return true;
    }
    bool isConnected()
    {
        cout << "[Connection] isConnected:true " << endl;
        return true;
    }
    bool disconnect()
    {
        cout << "[Connection] disconnected. " << endl;
        return true;
    }

    // Subscribe Qos=1
    bool subscribe(const char* topic)
    {
        cout << "[MQTT] subscribe:" << topic << endl;
        return true;
    }
    bool unsubscribe(const char* topic)
    {
        cout << "[MQTT] unsubscribe:" << topic << endl;
        return true;
    }

    bool publish(const char* topic, const uint8_t* msg, size_t len, bool retained)
    {
        cout << "[MQTT] publish:" << topic << "," << msg << "," << len << "," << retained << endl;
        return true;
    }

    uint32_t getUptimeNetwork()
    {
        cout << "[Connection] getUptimeNetwork:10" << endl;
        return 10;
    }
    uint32_t getUptimeMQTT()
    {
        cout << "[MQTT] getUptimeMQTT:20" << endl;
        return 20;
    }

    // len must be set to current msg length
    bool getMsg(char* topic, uint8_t* msg, size_t& len)
    {
        cout << "[MQTT] getMsg:false" << endl;
        return false;
    }
};

ConsoleAdapter apt("localhost", 1883, "", "");
Device dev(&apt, "root","test","testDevice","testFirmware");
Node node1("node1", "node1", "TestNode");
Property prop1("prop1", "prop1");

int main()
{
    dev.addNode(&node1);
    node1.addProp(&prop1);
    dev.begin();
    dev.ready();
    dev.loop();
    return 0;
}