#include "esphome/core/log.h"
#include "local_switch.h"

namespace esphome {
namespace empty_switch {

static const char *TAG = "local_switch.switch";

void EmptySwitch::write_state(bool state)
{
    _state = state;
    publish_state(state);
}

} //namespace empty_switch
} //namespace esphome