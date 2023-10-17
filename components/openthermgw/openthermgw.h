#include "esphome.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/switch/switch.h"

#include <OpenTherm.h>

#include <map>
#include <vector>

#ifndef OpenThermGW_h
#define OpenThermGW_h


namespace esphome {
namespace openthermgw {

static const char *LOGTOPIC = "openthermgw_component_21";
static const char *VERSION = "0.0.1.1";

class OpenthermGW: public PollingComponent
{
    private:

    protected:
    uint8_t master_in_pin_ = -1;
    uint8_t master_out_pin_ = -1;
    uint8_t slave_in_pin_ = -1;
    uint8_t slave_out_pin_ = -1;
    
    public:

    static OpenTherm *mOT;
    static OpenTherm *sOT;

    sensor::Sensor *master_in_pin_sensor = new sensor::Sensor();
    sensor::Sensor *master_out_pin_sensor = new sensor::Sensor();
    sensor::Sensor *slave_in_pin_sensor = new sensor::Sensor();
    sensor::Sensor *slave_out_pin_sensor = new sensor::Sensor();

    struct AcmeSensorInfo
    {
        int messageID;
        bool valueOnRequest;
        int valueType;
        sensor::Sensor *acmeSensor;
    };

    static std::map<int, std::vector<AcmeSensorInfo *> *> acme_sensor_map;

    //text_sensor::TextSensor *sensor_version;
    static binary_sensor::BinarySensor *sensor_status_slave_fault;
    static binary_sensor::BinarySensor *sensor_status_slave_CHmode;
    static binary_sensor::BinarySensor *sensor_status_slave_DHWmode;
    static binary_sensor::BinarySensor *sensor_status_slave_flame;
    static binary_sensor::BinarySensor *sensor_status_slave_cooling;
    static binary_sensor::BinarySensor *sensor_status_slave_CH2mode;
    static binary_sensor::BinarySensor *sensor_status_slave_diagnostic;
    static binary_sensor::BinarySensor *sensor_status_master_CHenable;
    static binary_sensor::BinarySensor *sensor_status_master_DHWenable;
    static binary_sensor::BinarySensor *sensor_status_master_coolingenable;
    static binary_sensor::BinarySensor *sensor_status_master_OTCactive;
    static binary_sensor::BinarySensor *sensor_status_master_CH2enable;

    static switch_::Switch *switch_dhw_pump_override;
    static switch_::Switch *switch_dhw_pump_override_mode;

    void set_master_in_pin(uint8_t pin);
    void set_master_out_pin(uint8_t pin);
    void set_slave_in_pin(uint8_t pin);
    void set_slave_out_pin(uint8_t pin);
    
    //void set_sensor_version(text_sensor::TextSensor *s) { sensor_version = s; }
    void set_sensor_status_slave_fault(binary_sensor::BinarySensor *s)      { sensor_status_slave_fault             = s; }
    void set_sensor_status_slave_CHmode(binary_sensor::BinarySensor *s)     { sensor_status_slave_CHmode            = s; }
    void set_sensor_status_slave_DHWmode(binary_sensor::BinarySensor *s)    { sensor_status_slave_DHWmode           = s; }
    void set_sensor_status_slave_flame(binary_sensor::BinarySensor *s)      { sensor_status_slave_flame             = s; }
    void set_sensor_status_slave_cooling(binary_sensor::BinarySensor *s)    { sensor_status_slave_cooling           = s; }
    void set_sensor_status_slave_CH2mode(binary_sensor::BinarySensor *s)    { sensor_status_slave_CH2mode           = s; }
    void set_sensor_status_slave_diagnostic(binary_sensor::BinarySensor *s) { sensor_status_slave_diagnostic        = s; }
    void set_sensor_status_master_CHenable(binary_sensor::BinarySensor *s)  { sensor_status_master_CHenable         = s; }
    void set_sensor_status_master_DHWenable(binary_sensor::BinarySensor *s) { sensor_status_master_DHWenable        = s; }
    void set_sensor_status_master_coolingenable(binary_sensor::BinarySensor *s) { sensor_status_master_coolingenable= s; }
    void set_sensor_status_master_OTCactive(binary_sensor::BinarySensor *s) { sensor_status_master_OTCactive        = s; }
    void set_sensor_status_master_CH2enable(binary_sensor::BinarySensor *s) { sensor_status_master_CH2enable        = s; }
    void set_switch_dhw_pump_override(switch_::Switch *s)                   { switch_dhw_pump_override              = s; }
    void set_switch_dhw_pump_override_mode(switch_::Switch *s)              { switch_dhw_pump_override_mode         = s; }
    void add_sensor_acme(sensor::Sensor *s, int messageid, bool valueonrequest, int valuetype);

    OpenthermGW();

    static void IRAM_ATTR mHandleInterrupt();
    static void IRAM_ATTR sHandleInterrupt();
    static void processRequest(unsigned long request, OpenThermResponseStatus status);

    void setup() override;
    void update() override;
    void loop() override;
};
/*
class Sensor_Version: public PollingComponent, public TextSensor
{
    public:
        Sensor_Version() : PollingComponent(3600000) {}

        void setup() override
        {
            publish_state(VERSION);
        }
        void update() override
        {
            publish_state(VERSION);
        }
}
*/
}
}

#endif // OpenThermGW_h