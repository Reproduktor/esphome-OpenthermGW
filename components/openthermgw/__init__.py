CODEOWNERS = ["@reproduktor/esphome-openthermgw"]

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, binary_sensor, switch
from esphome import pins
from esphome.const import *
from esphome.core import coroutine_with_priority
# from esphome.cpp_generator import RawExpression

CONF_MASTER_IN_PIN = "master_in_pin"
CONF_MASTER_OUT_PIN = "master_out_pin"
CONF_SLAVE_IN_PIN = "slave_in_pin"
CONF_SLAVE_OUT_PIN = "slave_out_pin"
CONF_OPENTHERM_ID = "opentherm_id"

opentherm_ns = cg.esphome_ns.namespace("openthermgw")
OpenThermGW = opentherm_ns.class_("OpenthermGW", cg.Component)

AUTO_LOAD = ['sensor', 'binary_sensor', 'switch']
MULTI_CONF = False

CONF_SENSOR_VERSION = "gw_version"

CONF_SENSOR_STATUS_MASTER_CHENABLE = "status_master_chenable"
CONF_SENSOR_STATUS_MASTER_DHWENABLE = "status_master_dhwenable"
CONF_SENSOR_STATUS_MASTER_COOLINGENABLE = "sensor_status_master_coolingenable"
CONF_SENSOR_STATUS_MASTER_OTCACTIVE = "sensor_status_master_otcactive"
CONF_SENSOR_STATUS_MASTER_CH2ENABLE = "sensor_status_master_ch2enable"
CONF_SENSOR_STATUS_SLAVE_FAULT = "status_slave_fault"
CONF_SENSOR_STATUS_SLAVE_CHMODE = "status_slave_chmode"
CONF_SENSOR_STATUS_SLAVE_DHWMODE = "status_slave_dhwmode"
CONF_SENSOR_STATUS_SLAVE_FLAME = "status_slave_flame"
CONF_SENSOR_STATUS_SLAVE_COOLING = "status_slave_cooling"
CONF_SENSOR_STATUS_SLAVE_CH2MODE = "status_slave_ch2mode"
CONF_SENSOR_STATUS_SLAVE_DIAGNOSTIC = "status_slave_diagnostic"
CONF_SENSOR_TEMP_BOILER = "temp_boiler"
CONF_SENSOR_TEMP_DHW = "temp_dhw"
CONF_SENSOR_MODULATIONLEVEL_BOILER = "modulationlevel_boiler"

CONF_SWITCH_DHW_PUMP_OVERRIDE = "dhw_pump_override"
CONF_SWITCH_DHW_PUMP_OVERRIDE_MODE = "dhw_pump_override_mode"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThermGW),
        cv.Required(CONF_MASTER_IN_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_MASTER_OUT_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_SLAVE_IN_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_SLAVE_OUT_PIN): pins.internal_gpio_input_pin_number,

        cv.Optional(CONF_SENSOR_STATUS_MASTER_CHENABLE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),
        cv.Optional(CONF_SENSOR_STATUS_MASTER_DHWENABLE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),
        cv.Optional(CONF_SENSOR_STATUS_MASTER_COOLINGENABLE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),
        cv.Optional(CONF_SENSOR_STATUS_MASTER_OTCACTIVE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),
        cv.Optional(CONF_SENSOR_STATUS_MASTER_CH2ENABLE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_EMPTY).extend(),


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

        cv.Optional(CONF_SENSOR_TEMP_BOILER): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT).extend(),
        cv.Optional(CONF_SENSOR_TEMP_DHW): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT).extend(),
        cv.Optional(CONF_SENSOR_MODULATIONLEVEL_BOILER): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT).extend(),
        
        cv.Optional(CONF_SWITCH_DHW_PUMP_OVERRIDE): switch.switch_schema().extend(),

        cv.Optional(CONF_SWITCH_DHW_PUMP_OVERRIDE_MODE): switch.switch_schema(
            device_class=DEVICE_CLASS_SWITCH).extend(),
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

    if CONF_SENSOR_TEMP_DHW in config:
        conf = config[CONF_SENSOR_TEMP_DHW]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_sensor_temp_dhw(sens))

    if CONF_SENSOR_MODULATIONLEVEL_BOILER in config:
        conf = config[CONF_SENSOR_MODULATIONLEVEL_BOILER]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_sensor_modulationlevel_boiler(sens))

    if CONF_SENSOR_STATUS_MASTER_CHENABLE in config:
        conf = config[CONF_SENSOR_STATUS_MASTER_CHENABLE]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_master_CHenable(sens))

    if CONF_SENSOR_STATUS_MASTER_DHWENABLE in config:
        conf = config[CONF_SENSOR_STATUS_MASTER_DHWENABLE]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_master_DHWenable(sens))

    if CONF_SENSOR_STATUS_MASTER_COOLINGENABLE in config:
        conf = config[CONF_SENSOR_STATUS_MASTER_COOLINGENABLE]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_master_coolingenable(sens))

    if CONF_SENSOR_STATUS_MASTER_OTCACTIVE in config:
        conf = config[CONF_SENSOR_STATUS_MASTER_OTCACTIVE]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_master_OTCactive(sens))

    if CONF_SENSOR_STATUS_MASTER_CH2ENABLE in config:
        conf = config[CONF_SENSOR_STATUS_MASTER_CH2ENABLE]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_master_CH2enable(sens))

    if CONF_SENSOR_STATUS_SLAVE_FAULT in config:
        conf = config[CONF_SENSOR_STATUS_SLAVE_FAULT]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_slave_fault(sens))

    if CONF_SENSOR_STATUS_SLAVE_CHMODE in config:
        conf = config[CONF_SENSOR_STATUS_SLAVE_CHMODE]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_slave_CHmode(sens))

    if CONF_SENSOR_STATUS_SLAVE_DHWMODE in config:
        conf = config[CONF_SENSOR_STATUS_SLAVE_DHWMODE]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_slave_DHWmode(sens))

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
        cg.add(var.set_sensor_status_slave_CH2mode(sens))

    if CONF_SENSOR_STATUS_SLAVE_DIAGNOSTIC in config:
        conf = config[CONF_SENSOR_STATUS_SLAVE_DIAGNOSTIC]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_sensor_status_slave_diagnostic(sens))


    if CONF_SWITCH_DHW_PUMP_OVERRIDE in config:
        confsw = config[CONF_SWITCH_DHW_PUMP_OVERRIDE]
        swtch = await switch.new_switch(confsw)
        cg.add(var.set_switch_dhw_pump_override(swtch))
    
    # if CONF_SWITCH_DHW_PUMP_OVERRIDE in config:
    #     vsw = cg.new_Pvariable(config[CONF_ID])
    #     await switch.register_switch(vsw, config)
    #     cg.add(var.set_switch_dhw_pump_override(vsw))

#    if CONF_SWITCH_DHW_PUMP_OVERRIDE_MODE in config:
#        confsw = config[CONF_SWITCH_DHW_PUMP_OVERRIDE_MODE]
#        swtch = await switch.new_switch(confsw)
#        cg.add(var.set_switch_dhw_pump_override_mode(swtch))

def opentherm_component_schema():
    """Create a schema for a OpenTherm component.
    :return: The OpenTherm component schema, `extend` this in your config schema.
    """
    schema = {
        cv.GenerateID(CONF_OPENTHERM_ID): cv.use_id(OpenThermGW),
    }
    return cv.Schema(schema)