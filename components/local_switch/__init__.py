import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID

local_switch_ns = cg.esphome_ns.namespace('local_switch')
LocalSwitch = local_switch_ns.class_('LocalSwitch', switch.Switch, cg.Component)

LOCAL_SWITCH_SCHEMA = switch.SWITCH_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(LocalSwitch),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await switch.register_switch(var, config)

def local_switch_schema():
    schema = {
        cv.GenerateID(): cv.declare_id(LocalSwitch),
    }
    return cv.Schema(schema)    