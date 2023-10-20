#include "openthermgw.h"

OpenTherm *esphome::openthermgw::OpenthermGW::mOT;
OpenTherm *esphome::openthermgw::OpenthermGW::sOT;
esphome::switch_::Switch *esphome::openthermgw::OpenthermGW::switch_dhw_pump_override;
esphome::switch_::Switch *esphome::openthermgw::OpenthermGW::switch_dhw_pump_override_mode;
std::map<int, std::vector<esphome::openthermgw::OpenthermGW::AcmeSensorInfo *> *> esphome::openthermgw::OpenthermGW::acme_sensor_map;
std::map<int, std::vector<esphome::openthermgw::OpenthermGW::AcmeBinarySensorInfo *> *> esphome::openthermgw::OpenthermGW::acme_binary_sensor_map;

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

        switch_dhw_pump_override            = nullptr;
        switch_dhw_pump_override_mode       = nullptr;
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
        ESP_LOGD(LOGTOPIC, "Opentherm request [MessageType: %s, DataID: %d, Data: %x]", mOT->messageTypeToString(mOT->getMessageType(request)), mOT->getDataID(request), request&0xffff);
        unsigned long response = mOT->sendRequest(request);
        if (response)
        {
            sOT->sendResponse(response);
            ESP_LOGD(LOGTOPIC, "Opentherm response [MessageType: %s, DataID: %d, Data: %x, status %s]", sOT->messageTypeToString(sOT->getMessageType(response)), sOT->getDataID(response), response&0xffff, sOT->statusToString(status));

            // acme
            std::vector<AcmeSensorInfo *> *pSensorList = acme_sensor_map[sOT->getDataID(response)];
            if(pSensorList != nullptr)
            {
                for(AcmeSensorInfo *pSensorInfo: *pSensorList)
                {
                    switch(pSensorInfo->valueType)
                    {
                        case 0: // u16
                        {
                            unsigned short value = sOT->getUInt(response);
                            pSensorInfo->acmeSensor->publish_state(value);
                            break;
                        }
                        case 1: // s16
                        {
                            short value = response & 0xffff;
                            pSensorInfo->acmeSensor->publish_state(value);
                            break;
                        }
                        case 2: // f16
                        {
                            float value = sOT->getFloat(response);
                            pSensorInfo->acmeSensor->publish_state(value);
                            break;
                        }
                        case 3: // u8LB
                        {
                            unsigned char value = response & 0x00ff;
                            pSensorInfo->acmeSensor->publish_state(value);
                            break;
                        }
                        case 4: // u8HB
                        {
                            unsigned char value = (response & 0xff00) >> 8;
                            pSensorInfo->acmeSensor->publish_state(value);
                            break;
                        }
                        case 5: // s8LB
                        {
                            signed char value = response & 0x00ff;
                            pSensorInfo->acmeSensor->publish_state(value);
                            break;
                        }
                        case 6: // s8HB
                        {
                            signed char value = (response & 0xff00) >> 8;
                            pSensorInfo->acmeSensor->publish_state(value);
                            break;
                        }
                        case 7: // RESPONSE
                        {
                            pSensorInfo->acmeSensor->publish_state((response >> 28) & 7);
                            break;
                        }
                    }

                }
            }

            // acme binary
            std::vector<AcmeBinarySensorInfo *> *pBinarySensorList = acme_binary_sensor_map[sOT->getDataID(response)];
            if(pBinarySensorList != nullptr)
            {
                for(AcmeBinarySensorInfo *pBinarySensorInfo: *pBinarySensorList)
                {
                    bool bitvalue = sOT->getUInt(response) & (1<<(pBinarySensorInfo->bit - 1));
                    pBinarySensorInfo->acmeSensor->publish_state(bitvalue);
                }
            }
        }
    }

    void OpenthermGW::add_sensor_acme(sensor::Sensor *s, int messageid, bool valueonrequest, int valuetype)
    {
        AcmeSensorInfo *pAcmeSensorInfo = new AcmeSensorInfo();
        pAcmeSensorInfo->messageID = messageid;
        pAcmeSensorInfo->valueOnRequest = valueonrequest;
        pAcmeSensorInfo->valueType = valuetype;
        pAcmeSensorInfo->acmeSensor = s;

        std::vector<AcmeSensorInfo *> *pSensorList = acme_sensor_map[pAcmeSensorInfo->messageID];
        if(pSensorList == nullptr)
        {
            pSensorList = new std::vector<AcmeSensorInfo *>();
            acme_sensor_map[pAcmeSensorInfo->messageID] = pSensorList;
        }
        pSensorList->push_back(pAcmeSensorInfo);
    }

    void OpenthermGW::add_sensor_acme_binary(binary_sensor::BinarySensor *s, int messageid, bool valueonrequest, int bit)
    {
        AcmeBinarySensorInfo *pAcmeBinarySensorInfo = new AcmeBinarySensorInfo();
        pAcmeBinarySensorInfo->messageID = messageid;
        pAcmeBinarySensorInfo->valueOnRequest = valueonrequest;
        pAcmeBinarySensorInfo->bit = bit;
        pAcmeBinarySensorInfo->acmeSensor = s;

        std::vector<AcmeBinarySensorInfo *> *pSensorList = acme_binary_sensor_map[pAcmeBinarySensorInfo->messageID];
        if(pSensorList == nullptr)
        {
            pSensorList = new std::vector<AcmeBinarySensorInfo *>();
            acme_binary_sensor_map[pAcmeBinarySensorInfo->messageID] = pSensorList;
        }
        pSensorList->push_back(pAcmeBinarySensorInfo);
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
        ESP_LOGD(LOGTOPIC, "acme messages handdled: %d", acme_sensor_map.size());
        ESP_LOGD(LOGTOPIC, "acme binary messages handdled: %d", acme_binary_sensor_map.size());
    }

    void OpenthermGW::loop()
    {
        sOT->process();        
    }


//////////////////////////////////////////////////////

    void OverrideBinarySwitch::setup()
    {
        this->state = this->get_initial_state_with_restore_mode().value_or(false);
    }

    void OverrideBinarySwitch::write_state(bool state)
    {
        if(state_ != state)
        {
            state_ = state;
            this->publish_state(state);
        }
    }

}
}