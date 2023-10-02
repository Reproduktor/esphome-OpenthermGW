#include "esphome.h"
#include "esphome/components/sensor/sensor.h"
#include "opentherm.h"

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

    void set_master_in_pin(uint8_t pin) { master_in_pin_ = pin; }
    void set_master_out_pin(uint8_t pin) { master_out_pin_ = pin; }
    void set_slave_in_pin(uint8_t pin) { slave_in_pin_ = pin; }
    void set_slave_out_pin(uint8_t pin) { slave_out_pin_ = pin; }
    
    OpenthermGW(): PollingComponent(10000)
    {
    }

    static void IRAM_ATTR mHandleInterrupt()
    {
        mOT->handleInterrupt();
    }

    static void IRAM_ATTR sHandleInterrupt()
    {
        sOT->handleInterrupt();
    }

    static void processRequest(unsigned long request, OpenThermResponseStatus status)
    {
        ESP_LOGD(LOGTOPIC, "Opentherm request [request: %d]", request);
        unsigned long response = mOT->sendRequest(request);
        if (response)
        {
            Serial.println("B" + String(response, HEX)); // slave/boiler response
            sOT->sendResponse(response);
            ESP_LOGD(LOGTOPIC, "Opentherm response [response: %d, status %s", response, sOT->statusToString(status));
        }
    }

    void setup() override
    {
        // This will be called once to set up the component
        // think of it as the setup() call in Arduino
        ESP_LOGD(LOGTOPIC, "Setup");

        master_in_pin_sensor->publish_state(master_in_pin_);
        master_out_pin_sensor->publish_state(master_out_pin_);
        slave_in_pin_sensor->publish_state(slave_in_pin_);
        slave_out_pin_sensor->publish_state(slave_out_pin_);

        mOT = new OpenTherm(slave_in_pin_, slave_out_pin_); // master OT is paired with SLAVE pins (thermostat)
        sOT = new OpenTherm(master_in_pin_, master_out_pin_, true);

        mOT->begin(mHandleInterrupt);
        sOT->begin(sHandleInterrupt, processRequest);
/*        mOT->begin([this]()
            {
                mOT->handleInterrupt();
            });
        sOT->begin([this]()
            {
                sOT->handleInterrupt();
            }, this->processRequest); */
    }

    void update() override
    {
        ESP_LOGD(LOGTOPIC, "update");
    }

    void loop() override
    {
        sOT->process();        
    }
};

OpenthermGW::OpenTherm *mOT;
OpenthermGW::OpenTherm *sOT;

}
}