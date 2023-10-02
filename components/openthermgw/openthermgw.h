#include "esphome.h"
#include "esphome/components/sensor/sensor.h"
#include "opentherm.h"

namespace esphome {
namespace openthermgw {

class OpenthermGW: public PollingComponent
{
    private:
    const char *LOGTOPIC = "openthermgw_component_13";

    protected:
    uint8_t master_in_pin_ = -1;
    uint8_t master_out_pin_ = -1;
    uint8_t slave_in_pin_ = -1;
    uint8_t slave_out_pin_ = -1;

    #define MODE_LISTEN_MASTER 0
    #define MODE_LISTEN_SLAVE 1

    int mode = 0;
    OpenthermData message;

    public:

    sensor::Sensor *master_in_pin_sensor = new sensor::Sensor();
    sensor::Sensor *master_out_pin_sensor = new sensor::Sensor();
    sensor::Sensor *slave_in_pin_sensor = new sensor::Sensor();
    sensor::Sensor *slave_out_pin_sensor = new sensor::Sensor();

    void set_master_in_pin(uint8_t pin) { master_in_pin_ = pin; }
    void set_master_out_pin(uint8_t pin) { master_out_pin_ = pin; }
    void set_slave_in_pin(uint8_t pin) { slave_in_pin_ = pin; }
    void set_slave_out_pin(uint8_t pin) { slave_out_pin_ = pin; }
    
    OpenthermGW(): PollingComponent(10000)
    {
    }

    void setup() override
    {
        // This will be called once to set up the component
        // think of it as the setup() call in Arduino
        ESP_LOGD(LOGTOPIC, "Setup");

        pinMode(master_in_pin_, INPUT);
        digitalWrite(master_in_pin_, HIGH); // pull up
        digitalWrite(master_out_pin_, HIGH);
        pinMode(master_out_pin_, OUTPUT); // low output = high current, high output = low current
        pinMode(slave_in_pin_, INPUT);
        digitalWrite(slave_in_pin_, HIGH); // pull up
        digitalWrite(slave_out_pin_, HIGH);
        pinMode(slave_out_pin_, OUTPUT); // low output = high voltage, high output = low voltage
        master_in_pin_sensor->publish_state(master_in_pin_);
        master_out_pin_sensor->publish_state(master_out_pin_);
        slave_in_pin_sensor->publish_state(slave_in_pin_);
        slave_out_pin_sensor->publish_state(slave_out_pin_);
    }

    void update() override
    {
        ESP_LOGD(LOGTOPIC, "update");

    }

    void loop() override
    {
        if (mode == MODE_LISTEN_MASTER)
        {
            if (OPENTHERM::isSent() || OPENTHERM::isIdle() || OPENTHERM::isError())
            {
                OPENTHERM::listen(master_in_pin_);
            }
            else if (OPENTHERM::getMessage(message))
            {
                //Serial.print(F("-> "));
                //OPENTHERM::printToSerial(message);
                //Serial.println();
                //ESP_LOGD(LOGTOPIC, "Message from thermostat type: %02hhx  id: %02hhx  HB: %02hhx  LB: %02hhx", message.type, message.id, message.valueHB, message.valueLB);
                OPENTHERM::send(slave_out_pin_, message); // forward message to boiler
                //ESP_LOGD(LOGTOPIC, "---> sent to boiler");//: %1", message.id);
                mode = MODE_LISTEN_SLAVE;
            }
        }
        else if (mode == MODE_LISTEN_SLAVE)
        {
            if (OPENTHERM::isSent())
            {
                //ESP_LOGD(LOGTOPIC, "waiting for response from boiler...");
                OPENTHERM::listen(slave_in_pin_, 800); // response need to be send back by boiler within 800ms
                //ESP_LOGD(LOGTOPIC, "... finished waiting.");
            }
            else if (OPENTHERM::getMessage(message))
            {
                //Serial.print(F("<- "));
                //OPENTHERM::printToSerial(message);
                //Serial.println();
                //Serial.println();
                //ESP_LOGD(LOGTOPIC, "Message from boiler type: %02hhx  id: %02hhx  HB: %02hhx  LB: %02hhx", message.type, message.id, message.valueHB, message.valueLB);
                OPENTHERM::send(master_out_pin_, message); // send message back to thermostat
                //ESP_LOGD(LOGTOPIC, "---> sent to thermostat");//: %1", message.id);
                mode = MODE_LISTEN_MASTER;
            }
            else if (OPENTHERM::isError())
            {
                ESP_LOGD(LOGTOPIC, "Message error");
                mode = MODE_LISTEN_MASTER;
                // Serial.println(F("<- Timeout"));
                // Serial.println();
            }
        }
    }
};

}
}