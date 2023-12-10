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

static const char *TAG = "openthermgw_component";
static const char *VERSION = "0.0.1.2";

class SimpleNumber : public number::Number, public Component
{
public:
    void setup() override;
    void dump_config() override;
    float get_setup_priority() const override { return setup_priority::PROCESSOR; }

    Trigger<float> *get_set_trigger() const { return set_trigger_; }
    void set_initial_value(float initial_value) { initial_value_ = initial_value; }
    void set_restore_value(bool restore_value) { this->restore_value_ = restore_value; }

 protected:
    void control(float value) override;
    float initial_value_{NAN};
    bool restore_value_{true};
    Trigger<float> *set_trigger_ = new Trigger<float>();

    ESPPreferenceObject pref_;
};

class SimpleSwitch : public switch_::Switch, public Component
{
public:
    SimpleSwitch();

    void setup() override;

protected:
    bool state_ {false};
    void write_state(bool state) override;
};

class OverrideBinarySwitch : public switch_::Switch, public Component
{
public:
    OverrideBinarySwitch();

    void setup() override;

protected:
    bool state_ {false};
    void write_state(bool state) override;
};


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

    struct AcmeBinarySensorInfo
    {
        int messageID;
        bool valueOnRequest;
        int bit;
        binary_sensor::BinarySensor *acmeSensor;
    };

    struct OverrideBinarySwitchInfo
    {
        int messageID;
        bool valueOnRequest;
        int bit;
        OverrideBinarySwitch *binaryswitch;
        openthermgw::SimpleSwitch *valueswitch;
    };

    struct OverrideNumericSwitchInfo
    {
        int messageID;
        bool valueOnRequest;
        int valueType;
        OverrideBinarySwitch *binaryswitch;
        openthermgw::SimpleNumber *valuenumber;
    };

    static std::map<int, std::vector<AcmeSensorInfo *> *> acme_sensor_map;
    static std::map<int, std::vector<AcmeBinarySensorInfo *> *> acme_binary_sensor_map;
    static std::map<int, std::vector<OverrideBinarySwitchInfo *> *> override_binary_switch_map;
    static std::map<int, std::vector<OverrideNumericSwitchInfo *> *> override_numeric_switch_map;

    static switch_::Switch *switch_dhw_pump_override;
    static switch_::Switch *switch_dhw_pump_override_mode;

    void set_master_in_pin(uint8_t pin);
    void set_master_out_pin(uint8_t pin);
    void set_slave_in_pin(uint8_t pin);
    void set_slave_out_pin(uint8_t pin);
    
    void set_switch_dhw_pump_override(switch_::Switch *s)                   { switch_dhw_pump_override              = s; }
    void set_switch_dhw_pump_override_mode(switch_::Switch *s)              { switch_dhw_pump_override_mode         = s; }
    void add_sensor_acme(sensor::Sensor *s, int messageid, bool valueonrequest, int valuetype);
    void add_sensor_acme_binary(binary_sensor::BinarySensor *s, int messageid, bool valueonrequest, int bit);
    void add_override_switch(openthermgw::OverrideBinarySwitch *s, int messageid, bool valueonrequest, int bit, openthermgw::SimpleSwitch *v);
    void add_override_numeric_switch(openthermgw::OverrideBinarySwitch *s, int messageid, bool valueonrequest, int valuetype, openthermgw::SimpleNumber *v);

    OpenthermGW();

    static void IRAM_ATTR mHandleInterrupt();
    static void IRAM_ATTR sHandleInterrupt();
    static void processRequest(unsigned long request, OpenThermResponseStatus status);

    void setup() override;
    void update() override;
    void loop() override;
};

}
}

#endif // OpenThermGW_h