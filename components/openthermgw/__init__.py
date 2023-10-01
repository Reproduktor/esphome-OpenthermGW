CODEOWNERS = ["@reproduktor/esphome-openthermgw"]

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor
from esphome import pins
from esphome.const import (
    CONF_ID,
)
from esphome.core import coroutine_with_priority
from esphome.cpp_generator import RawExpression

CONF_THERMOSTAT_IN_PIN = "thermostat_in_pin"
CONF_THERMOSTAT_OUT_PIN = "thermostat_out_pin"
CONF_BOILER_IN_PIN = "boiler_in_pin"
CONF_BOILER_OUT_PIN = "boiler_out_pin"
CONF_OPENTHERM_ID = "opentherm_id"

opentherm_ns = cg.esphome_ns.namespace("OPENTHERM")
OpenThermGW = opentherm_ns.class_("OpenthermGW", cg.Component, cg.esphome_ns.namespace("text_sensor").class_("TextSensor"))


MULTI_CONF = False
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThermGW),
        cv.Required(CONF_THERMOSTAT_IN_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_THERMOSTAT_OUT_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_BOILER_IN_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_BOILER_OUT_PIN): pins.internal_gpio_input_pin_number,
    }
).extend(cv.COMPONENT_SCHEMA)


@coroutine_with_priority(2.0)
async def to_code(config):

    cg.add_global(opentherm_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_thermostat_in_pin(config[CONF_THERMOSTAT_IN_PIN]))
    cg.add(var.set_thermostat_out_pin(config[CONF_THERMOSTAT_OUT_PIN]))
    cg.add(var.set_boiler_in_pin(config[CONF_BOILER_IN_PIN]))
    cg.add(var.set_boiler_out_pin(config[CONF_BOILER_OUT_PIN]))

    #cg.add_library("ihormelnyk/OpenTherm Library", "1.1.3")
    #cg.add_library("https://github.com/rsciriano/opentherm_library.git#functional_callbacks", None)

def opentherm_component_schema():
    """Create a schema for a OpenTherm component.
    :return: The OpenTherm component schema, `extend` this in your config schema.
    """
    schema = {
        cv.GenerateID(CONF_OPENTHERM_ID): cv.use_id(OpenThermGW),
    }
    return cv.Schema(schema)

#async def set_hotwater_climate(var, config):
#    parent = await cg.get_variable(config[CONF_OPENTHERM_ID])
#    cg.add(parent.set_hotWaterClimate(var))

#async def set_heatingwater_climate(var, config):
#    parent = await cg.get_variable(config[CONF_OPENTHERM_ID])
#   cg.add(parent.set_heatingWaterClimate(var))