#include "openthermgw.h"

OpenTherm *esphome::openthermgw::OpenthermGW::mOT;
OpenTherm *esphome::openthermgw::OpenthermGW::sOT;
esphome::sensor::Sensor *esphome::openthermgw::OpenthermGW::sensor_temp_boiler;
esphome::sensor::Sensor *esphome::openthermgw::OpenthermGW::sensor_temp_dhw;
esphome::sensor::Sensor *esphome::openthermgw::OpenthermGW::sensor_modulationlevel_boiler;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_fault;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_CHmode;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_DHWmode;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_flame;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_cooling;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_CH2mode;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_diagnostic;

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

        sensor_temp_boiler                  = nullptr;
        sensor_modulationlevel_boiler       = nullptr;
        sensor_status_slave_fault           = nullptr;
        sensor_status_slave_CHmode          = nullptr;
        sensor_status_slave_DHWmode         = nullptr;
        sensor_status_slave_flame           = nullptr;
        sensor_status_slave_cooling         = nullptr;
        sensor_status_slave_CH2mode         = nullptr;
        sensor_status_slave_diagnostic      = nullptr;
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

                    bool b = response & 0x01; // bit 0, Fault indication
                    if(sensor_status_slave_fault != nullptr)
                        sensor_status_slave_fault->publish_state(b);
                    b = response & 0x02; // bit 1, CH mode
                    if(sensor_status_slave_CHmode != nullptr)
                        sensor_status_slave_CHmode->publish_state(b);
                    b = response & 0x4; // bit 2, DHW mode
                    if(sensor_status_slave_DHWmode != nullptr)
                        sensor_status_slave_DHWmode->publish_state(b);
                    b = response & 0x8; // bit 3, Flame On
                    if(sensor_status_slave_flame != nullptr)
                        sensor_status_slave_flame->publish_state(b);
                    b = response & 0x10; // bit 4, Cooling status
                    if(sensor_status_slave_cooling != nullptr)
                        sensor_status_slave_cooling->publish_state(b);
                    b = response & 0x20; // bit 5, CH2 mode
                    if(sensor_status_slave_CH2mode != nullptr)
                        sensor_status_slave_CH2mode->publish_state(b);
                    b = response & 0x40; // bit 6, Diagnostic indication
                    if(sensor_status_slave_diagnostic != nullptr)
                        sensor_status_slave_diagnostic->publish_state(b);

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
                case Tdhw:
                {
                    float f = sOT->getFloat(response);
                    ESP_LOGD(LOGTOPIC, "Opentherm response - Tdhw [%f]", f);
        
                    if(sensor_temp_dhw != nullptr)
                        sensor_temp_dhw->publish_state(f);

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