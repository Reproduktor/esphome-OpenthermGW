#include "local_switch.h"

namespace esphome {
namespace empty_switch {

static const char *TAG = "local_switch.switch";

void LocalSwitch::setup()
{
    set_restore_mode(SwitchRestoreMode::SWITCH_RESTORE_DEFAULT_OFF);
}

void LocalSwitch::write_state(bool state)
{
    _state = state;
    publish_state(state);
}

} //namespace empty_switch
} //namespace esphome