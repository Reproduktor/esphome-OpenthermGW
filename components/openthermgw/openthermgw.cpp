#include "openthermgw.h"

OpenTherm *esphome::openthermgw::OpenthermGW::mOT;
OpenTherm *esphome::openthermgw::OpenthermGW::sOT;
esphome::sensor::Sensor *esphome::openthermgw::OpenthermGW::sensor_temp_boiler;
esphome::sensor::Sensor *esphome::openthermgw::OpenthermGW::sensor_modulationlevel_boiler;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_flame;

namespace esphome {
namespace openthermgw {

    void OpenthermGW::set_master_in_pin(uint8_t pin) { master_in_pin_ = pin; }
    void OpenthermGW::set_master_out_pin(uint8_t pin) { master_out_pin_ = pin; }
    void OpenthermGW::set_slave_in_pin(uint8_t pin) { slave_in_pin_ = pin; }
    void OpenthermGW::set_slave_out_pin(uint8_t pin) { slave_out_pin_ = pin; }
    
    OpenthermGW::OpenthermGW(): PollingComponent(10000)
    {
        mOT = nullptr;
        sOT = nullptr;
        sensor_temp_boiler = nullptr;
        sensor_modulationlevel_boiler = nullptr;
        sensor_status_slave_flame = nullptr;
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
            sOT->sendResponse(response);
            ESP_LOGD(LOGTOPIC, "Opentherm response [response: %d, status %s", response, sOT->statusToString(status));
            switch(sOT->getDataID(response))
            {
                case 0:
                {
                    ESP_LOGD(LOGTOPIC, "Opentherm response - Status [%x]", response & 0xff);

                    bool b = response & 0x8; // bit 3, Flame On
                    if(sensor_status_slave_flame != nullptr)
                        sensor_status_slave_flame->publish_state(b);

                    break;
                }
                case Tboiler:
                {
                    float f = sOT->getFloat(response);
                    ESP_LOGD(LOGTOPIC, "Opentherm response - Tboiler [%f]", f);
        
                    if(sensor_temp_boiler != nullptr)
                        sensor_temp_boiler->publish_state(f);

                    break;
                }
                case RelModLevel:
                {
                    float f = sOT->getFloat(response);
                    ESP_LOGD(LOGTOPIC, "Opentherm response - RelModLevel [%f]", f);
        
                    if(sensor_temp_boiler != nullptr)
                        sensor_modulationlevel_boiler->publish_state(f);

                    break;
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
    }

    void OpenthermGW::loop()
    {
        sOT->process();        
    }


}
}