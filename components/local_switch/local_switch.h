#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace local_switch {

class LocalSwitch : public switch_::Switch, public Component
{
    private:
        bool _state = false;

    public:
        void setup() override;
        void write_state(bool state) override;
        bool getState() { return _state; }
};

} // namespace local_switch
} //namespace esphome