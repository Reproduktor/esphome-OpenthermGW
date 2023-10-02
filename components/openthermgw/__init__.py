CODEOWNERS = ["@reproduktor/esphome-openthermgw"]

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor, binary_sensor
from esphome import pins
from esphome.const import *
from esphome.core import coroutine_with_priority
from esphome.cpp_generator import RawExpression

CONF_MASTER_IN_PIN = "master_in_pin"
CONF_MASTER_OUT_PIN = "master_out_pin"
CONF_SLAVE_IN_PIN = "slave_in_pin"
CONF_SLAVE_OUT_PIN = "slave_out_pin"
CONF_OPENTHERM_ID = "opentherm_id"

opentherm_ns = cg.esphome_ns.namespace("openthermgw")
OpenThermGW = opentherm_ns.class_("OpenthermGW", cg.Component)

AUTO_LOAD = ['binary_sensor']
MULTI_CONF = False

CONF_SENSOR_VERSION = "version"
CONF_SENSOR_TEMP_BOILER = "temp_boiler"
CONF_SENSOR_MODULATIONLEVEL_BOILER = "modulationlevel_boiler"
CONF_SENSOR_STATUS_SLAVE_FAULT = "status_slave_fault"
CONF_SENSOR_STATUS_SLAVE_CHMODE = "status_slave_chmode"
CONF_SENSOR_STATUS_SLAVE_DHWMODE = "status_slave_dhwmode"
CONF_SENSOR_STATUS_SLAVE_FLAME = "status_slave_flame"
CONF_SENSOR_STATUS_SLAVE_COOLING = "status_slave_cooling"
CONF_SENSOR_STATUS_SLAVE_CH2MODE = "status_slave_ch2mode"
CONF_SENSOR_STATUS_SLAVE_DIAGNOSTIC = "status_slave_diagnostic"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThermGW),
        cv.Required(CONF_MASTER_IN_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_MASTER_OUT_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_SLAVE_IN_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_SLAVE_OUT_PIN): pins.internal_gpio_input_pin_number,
        cv.Optional(CONF_SENSOR_TEMP_BOILER): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT).extend(),
        cv.Optional(CONF_SENSOR_MODULATIONLEVEL_BOILER): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT).extend(),
        cv.Optional(CONF_SENSOR_STATUS_SLAVE_FAULT): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),
        cv.Optional(CONF_SENSOR_STATUS_SLAVE_CHMODE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),
        cv.Optional(CONF_SENSOR_STATUS_SLAVE_DHWMODE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),
        cv.Optional(CONF_SENSOR_STATUS_SLAVE_FLAME): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),
        cv.Optional(CONF_SENSOR_STATUS_SLAVE_COOLING): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),
        cv.Optional(CONF_SENSOR_STATUS_SLAVE_CH2MODE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),
        cv.Optional(CONF_SENSOR_STATUS_SLAVE_DIAGNOSTIC): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),
    }
).extend(cv.COMPONENT_SCHEMA)



@coroutine_with_priority(2.0)
async def to_code(config):

    cg.add_global(opentherm_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_master_in_pin(config[CONF_MASTER_IN_PIN]))
    cg.add(var.set_master_out_pin(config[CONF_MASTER_OUT_PIN]))
    cg.add(var.set_slave_in_pin(config[CONF_SLAVE_IN_PIN]))
    cg.add(var.set_slave_out_pin(config[CONF_SLAVE_OUT_PIN]))

    cg.add_library("ihormelnyk/OpenTherm Library", "1.1.4")

    if CONF_SENSOR_TEMP_BOILER in config:
        conf = config[CONF_SENSOR_TEMP_BOILER]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_sensor_temp_boiler(sens))

    if CONF_SENSOR_MODULATIONLEVEL_BOILER in config:
        conf = config[CONF_SENSOR_MODULATIONLEVEL_BOILER]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_sensor_modulationlevel_boiler(sens))

    if CONF_SENSOR_STATUS_SLAVE_FAULT in config:
        conf = config[CONF_SENSOR_STATUS_SLAVE_FAULT]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_slave_fault(sens))

    if CONF_SENSOR_STATUS_SLAVE_CHMODE in config:
        conf = config[CONF_SENSOR_STATUS_SLAVE_CHMODE]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_slave_chmode(sens))

    if CONF_SENSOR_STATUS_SLAVE_DHWMODE in config:
        conf = config[CONF_SENSOR_STATUS_SLAVE_DHWMODE]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_slave_dhwmode(sens))

    if CONF_SENSOR_STATUS_SLAVE_FLAME in config:
        conf = config[CONF_SENSOR_STATUS_SLAVE_FLAME]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_slave_flame(sens))

    if CONF_SENSOR_STATUS_SLAVE_COOLING in config:
        conf = config[CONF_SENSOR_STATUS_SLAVE_COOLING]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_slave_cooling(sens))

    if CONF_SENSOR_STATUS_SLAVE_CH2MODE in config:
        conf = config[CONF_SENSOR_STATUS_SLAVE_CH2MODE]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_slave_ch2mode(sens))

    if CONF_SENSOR_STATUS_SLAVE_DIAGNOSTIC in config:
        conf = config[CONF_SENSOR_STATUS_SLAVE_DIAGNOSTIC]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_slave_diagnostic(sens))


def opentherm_component_schema():
    """Create a schema for a OpenTherm component.
    :return: The OpenTherm component schema, `extend` this in your config schema.
    """
    schema = {
        cv.GenerateID(CONF_OPENTHERM_ID): cv.use_id(OpenThermGW),
    }
    return cv.Schema(schema)