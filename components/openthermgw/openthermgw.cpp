#include "openthermgw.h"

OpenTherm *esphome::openthermgw::OpenthermGW::mOT;
OpenTherm *esphome::openthermgw::OpenthermGW::sOT;
esphome::sensor::Sensor *esphome::openthermgw::OpenthermGW::sensor_temp_boiler;
esphome::sensor::Sensor *esphome::openthermgw::OpenthermGW::sensor_modulationlevel_boiler;

namespace esphome {
namespace openthermgw {

    void OpenthermGW::set_master_in_pin(uint8_t pin) { master_in_pin_ = pin; }
    void OpenthermGW::set_master_out_pin(uint8_t pin) { master_out_pin_ = pin; }
    void OpenthermGW::set_slave_in_pin(uint8_t pin) { slave_in_pin_ = pin; }
    void OpenthermGW::set_slave_out_pin(uint8_t pin) { slave_out_pin_ = pin; }
    
    OpenthermGW::OpenthermGW(): PollingComponent(10000)
    {
        sensor_temp_boiler = nullptr;
        mOT = nullptr;
        sOT = nullptr;
    }

    void IRAM_ATTR OpenthermGW::mHandleInterrupt()
    {
        mOT->handleInterrupt();
    }

    void IRAM_ATTR OpenthermGW::sHandleInterrupt()
    {
        sOT->handleInterrupt();
    }

    void OpenthermGW::processRequest(unsigned long request, OpenThermResponseStatus status)
    {
        ESP_LOGD(LOGTOPIC, "Opentherm request [request: %d]", request);
        unsigned long response = mOT->sendRequest(request);
        if (response)
        {
            //Serial.println("B" + String(response, HEX)); // slave/boiler response
            sOT->sendResponse(response);
            ESP_LOGD(LOGTOPIC, "Opentherm response [response: %d, status %s", response, sOT->statusToString(status));
            switch(sOT->getDataID(response))
            {
                Tboiler:
                {
                    float f = sOT->getFloat(response);
                    ESP_LOGD(LOGTOPIC, "Opentherm response - Tboiler [%f]", f);
        
                    if(sensor_temp_boiler != nullptr)
                        sensor_temp_boiler->publish_state(f);

                    break;
                }
                RelModLevel:
                {
                    float f = sOT->getFloat(response);
                    ESP_LOGD(LOGTOPIC, "Opentherm response - RelModLevel [%f]", f);
        
                    if(sensor_temp_boiler != nullptr)
                        sensor_modulationlevel_boiler->publish_state(f);
                }
            }
        }
    }

    void OpenthermGW::setup()
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
    }

    void OpenthermGW::update()
    {
        ESP_LOGD(LOGTOPIC, "update");
        // if(sensor_temp_boiler != nullptr)
        //     sensor_temp_boiler->publish_state(28.3);
    }

    void OpenthermGW::loop()
    {
        sOT->process();        
    }


}
}