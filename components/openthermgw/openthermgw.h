#include "esphome.h"
#include "opentherm.h"

namespace esphome {
namespace openthermgw {

class OpenthermGW: public PollingComponent
{
    private:
    const char *TAG = "openthermgw_component";
    const char *LOGCOMP = "openthermgw_component02";

    protected:
    uint8_t thermostat_in_pin_ = -1;
    uint8_t thermostat_out_pin_ = -1;
    uint8_t boiler_in_pin_ = -1;
    uint8_t boiler_out_pin_ = -1;

    #define MODE_LISTEN_MASTER 0
    #define MODE_LISTEN_SLAVE 1

    int mode = 0;
    OpenthermData message;

    public:
    void set_thermostat_in_pin(uint8_t pin) { thermostat_in_pin_ = pin; }
    void set_thermostat_out_pin(uint8_t pin) { thermostat_out_pin_ = pin; }
    void set_boiler_in_pin(uint8_t pin) { boiler_in_pin_ = pin; }
    void set_boiler_out_pin(uint8_t pin) { boiler_out_pin_ = pin; }
    
    OpenthermGW(): PollingComponent(3000)
    {
    }

    void setup() override
    {
        // This will be called once to set up the component
        // think of it as the setup() call in Arduino
        ESP_LOGD(TAG, "Setup");

        pinMode(thermostat_in_pin_, INPUT);
        digitalWrite(thermostat_in_pin_, HIGH); // pull up
        digitalWrite(thermostat_out_pin_, HIGH);
        pinMode(thermostat_out_pin_, OUTPUT); // low output = high current, high output = low current
        pinMode(boiler_in_pin_, INPUT);
        digitalWrite(boiler_in_pin_, HIGH); // pull up
        digitalWrite(boiler_out_pin_, HIGH);
        pinMode(boiler_out_pin_, OUTPUT); // low output = high voltage, high output = low voltage
    }

    void update() override
    {
        ESP_LOGD(LOGCOMP, "update");
    }

    void loop() override
    {
        if (mode == MODE_LISTEN_MASTER)
        {
            if (OPENTHERM::isSent() || OPENTHERM::isIdle() || OPENTHERM::isError())
            {
                OPENTHERM::listen(thermostat_in_pin_);
            }
            else if (OPENTHERM::getMessage(message))
            {
                //Serial.print(F("-> "));
                //OPENTHERM::printToSerial(message);
                //Serial.println();
                ESP_LOGD(LOGCOMP, "Message from thermostat");//: %1", message.id);

                OPENTHERM::send(boiler_out_pin_, message); // forward message to boiler
                mode = MODE_LISTEN_SLAVE;
            }
        }
        else if (mode == MODE_LISTEN_SLAVE)
        {
            if (OPENTHERM::isSent())
            {
                OPENTHERM::listen(boiler_in_pin_, 800); // response need to be send back by boiler within 800ms
            }
            else if (OPENTHERM::getMessage(message))
            {
                //Serial.print(F("<- "));
                //OPENTHERM::printToSerial(message);
                //Serial.println();
                //Serial.println();
                ESP_LOGD(LOGCOMP, "Message from boiler");// %1", message.id);
                OPENTHERM::send(thermostat_out_pin_, message); // send message back to thermostat
                mode = MODE_LISTEN_MASTER;
            }
            else if (OPENTHERM::isError())
            {
                ESP_LOGD(LOGCOMP, "Message error");
                mode = MODE_LISTEN_MASTER;
                // Serial.println(F("<- Timeout"));
                // Serial.println();
            }
        }
    }
};

}
}