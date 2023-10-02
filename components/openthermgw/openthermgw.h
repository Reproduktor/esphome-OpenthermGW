#include "esphome.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

#include <OpenTherm.h>

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

    //text_sensor::TextSensor *sensor_version;
    static sensor::Sensor *sensor_temp_boiler;
    static sensor::Sensor *sensor_modulationlevel_boiler;
    static binary_sensor::BinarySensor *sensor_status_slave_fault;
    static binary_sensor::BinarySensor *sensor_status_slave_CHmode;
    static binary_sensor::BinarySensor *sensor_status_slave_DHWmode;
    static binary_sensor::BinarySensor *sensor_status_slave_flame;
    static binary_sensor::BinarySensor *sensor_status_slave_cooling;
    static binary_sensor::BinarySensor *sensor_status_slave_CH2mode;
    static binary_sensor::BinarySensor *sensor_status_slave_diagnostic;

    void set_master_in_pin(uint8_t pin);
    void set_master_out_pin(uint8_t pin);
    void set_slave_in_pin(uint8_t pin);
    void set_slave_out_pin(uint8_t pin);
    
    //void set_sensor_version(text_sensor::TextSensor *s) { sensor_version = s; }
    void set_sensor_temp_boiler(sensor::Sensor *s) { sensor_temp_boiler = s; }
    void set_sensor_modulationlevel_boiler(sensor::Sensor *s)                       { sensor_modulationlevel_boiler         = s; }
    void set_sensor_status_slave_fault(binary_sensor::BinarySensor *s)       { sensor_status_slave_fault             = s; }
    void set_sensor_status_slave_CHmode(binary_sensor::BinarySensor *s)      { sensor_status_slave_CHmode            = s; }
    void set_sensor_status_slave_DHWmode(binary_sensor::BinarySensor *s)     { sensor_status_slave_DHWmode           = s; }
    void set_sensor_status_slave_flame(binary_sensor::BinarySensor *s)       { sensor_status_slave_flame             = s; }
    void set_sensor_status_slave_cooling(binary_sensor::BinarySensor *s)     { sensor_status_slave_cooling           = s; }
    void set_sensor_status_slave_CH2mode(binary_sensor::BinarySensor *s)     { sensor_status_slave_CH2mode           = s; }
    void set_sensor_status_slave_diagnostic(binary_sensor::BinarySensor *s)  { sensor_status_slave_diagnostic        = s; }

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