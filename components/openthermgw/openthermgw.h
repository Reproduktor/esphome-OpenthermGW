#include "esphome.h"
#include "esphome/components/sensor/sensor.h"
#include "opentherm.h"

#ifndef OpenThermGW_h
#define OpenThermGW_h


namespace esphome {
namespace openthermgw {

static const char *LOGTOPIC = "openthermgw_component_20";

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

    void set_master_in_pin(uint8_t pin);
    void set_master_out_pin(uint8_t pin);
    void set_slave_in_pin(uint8_t pin);
    void set_slave_out_pin(uint8_t pin);
    
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