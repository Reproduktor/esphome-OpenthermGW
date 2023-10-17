#include "openthermgw.h"

OpenTherm *esphome::openthermgw::OpenthermGW::mOT;
OpenTherm *esphome::openthermgw::OpenthermGW::sOT;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_master_CHenable;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_master_DHWenable;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_master_coolingenable;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_master_OTCactive;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_master_CH2enable;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_fault;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_CHmode;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_DHWmode;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_flame;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_cooling;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_CH2mode;
esphome::binary_sensor::BinarySensor *esphome::openthermgw::OpenthermGW::sensor_status_slave_diagnostic;
esphome::switch_::Switch *esphome::openthermgw::OpenthermGW::switch_dhw_pump_override;
esphome::switch_::Switch *esphome::openthermgw::OpenthermGW::switch_dhw_pump_override_mode;
std::map<int, std::vector<esphome::openthermgw::OpenthermGW::AcmeSensorInfo *> *> esphome::openthermgw::OpenthermGW::acme_sensor_map;

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

        sensor_status_slave_fault           = nullptr;
        sensor_status_slave_CHmode          = nullptr;
        sensor_status_slave_DHWmode         = nullptr;
        sensor_status_slave_flame           = nullptr;
        sensor_status_slave_cooling         = nullptr;
        sensor_status_slave_CH2mode         = nullptr;
        sensor_status_slave_diagnostic      = nullptr;

        sensor_status_master_CHenable       = nullptr;
        sensor_status_master_DHWenable      = nullptr;
        sensor_status_master_coolingenable  = nullptr;
        sensor_status_master_OTCactive      = nullptr;
        sensor_status_master_CH2enable      = nullptr;

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

            if(sOT->getMessageType(response) == READ_ACK)
            {

                // hardocded
                switch(sOT->getDataID(response))
                {
                    case 0:
                    {
                        ESP_LOGD(LOGTOPIC, "Opentherm response - Status [%x]", response & 0xffff);

                        bool b;

                        // Master status - HB
                        b = response & 0x0100; // bit 0, CH enable
                        if(sensor_status_master_CHenable != nullptr)
                            sensor_status_master_CHenable->publish_state(b);
                        b = response & 0x0200; // bit 1, DHW enable
                        if(sensor_status_master_DHWenable != nullptr)
                            sensor_status_master_DHWenable->publish_state(b);
                        b = response & 0x0400; // bit 2, Cooling enable
                        if(sensor_status_master_coolingenable != nullptr)
                            sensor_status_master_coolingenable->publish_state(b);
                        b = response & 0x0800; // bit 3, OTC active
                        if(sensor_status_master_OTCactive != nullptr)
                            sensor_status_master_OTCactive->publish_state(b);
                        b = response & 0x1000; // bit 4, CH2 enable
                        if(sensor_status_master_CH2enable != nullptr)
                            sensor_status_master_CH2enable->publish_state(b);


                        // Slave status - LB
                        b = response & 0x01; // bit 0, Fault indication
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
                }
            }// if READ_ACK
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
    }

    void OpenthermGW::loop()
    {
        sOT->process();        
    }


}
}