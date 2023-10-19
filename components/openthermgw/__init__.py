CODEOWNERS = ["@reproduktor/esphome-openthermgw"]

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, binary_sensor
from esphome import pins
from esphome.const import *
from esphome.core import coroutine_with_priority

CONF_MASTER_IN_PIN = "master_in_pin"
CONF_MASTER_OUT_PIN = "master_out_pin"
CONF_SLAVE_IN_PIN = "slave_in_pin"
CONF_SLAVE_OUT_PIN = "slave_out_pin"
CONF_OPENTHERM_ID = "opentherm_id"

opentherm_ns = cg.esphome_ns.namespace("openthermgw")
local_switch_ns = cg.esphome_ns.namespace("local_switch")

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

CONF_SENSOR_ACME_OT_LIST = "acme_opentherm_sensor_list"
CONF_SENSOR_ACME_OT_MESSAGE_ID = "message_id"
CONF_SENSOR_ACME_OT_VALUE_ON_REQUEST = "value_on_request"
CONF_SENSOR_ACME_OT_VALUE_TYPE = "value_type"
CONF_SCHEMA_ACME_OT = sensor.sensor_schema().extend(
    {
        cv.Required(CONF_SENSOR_ACME_OT_MESSAGE_ID): cv.positive_int,
        cv.Optional(CONF_SENSOR_ACME_OT_VALUE_ON_REQUEST, default='false'): cv.boolean,
        cv.Optional(CONF_SENSOR_ACME_OT_VALUE_TYPE, default=0): cv.int_range(0, 7), # 0=u16, 1=s16, 2=f16, 3=u8LB, 4=u8HB, 5=s8LB, 6=s8HB, 7=REQUEST/RESPONSE
    }
    )

CONF_SENSOR_ACME_OT_BINARY_LIST = "acme_opentherm_binary_sensors"
CONF_SENSOR_ACME_OT_BINARY_BIT = "bit"
CONF_SCHEMA_ACME_OT_BINARY = sensor.sensor_schema().extend(
    {
        cv.Required(CONF_SENSOR_ACME_OT_MESSAGE_ID): cv.positive_int,
        cv.Optional(CONF_SENSOR_ACME_OT_VALUE_ON_REQUEST, default='false'): cv.boolean,
        cv.Required(CONF_SENSOR_ACME_OT_BINARY_BIT): cv.int_range(1, 16), #1-16 bit index
    }
    )


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

        cv.Optional(CONF_SENSOR_ACME_OT_LIST): cv.All(
            cv.ensure_list(CONF_SCHEMA_ACME_OT), cv.Length(min=1, max=200)
            ),
        cv.Optional(CONF_SENSOR_ACME_OT_BINARY_LIST): cv.All(
            cv.ensure_list(CONF_SCHEMA_ACME_OT_BINARY), cv.Length(min=1, max=200)
            ),
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

    if CONF_SENSOR_ACME_OT_LIST in config:
        for messagesensor in config[CONF_SENSOR_ACME_OT_LIST]:
            sens = await sensor.new_sensor(messagesensor)
            cg.add(var.add_sensor_acme(sens, messagesensor[CONF_SENSOR_ACME_OT_MESSAGE_ID], messagesensor[CONF_SENSOR_ACME_OT_VALUE_ON_REQUEST], messagesensor[CONF_SENSOR_ACME_OT_VALUE_TYPE]))

    if CONF_SENSOR_ACME_OT_BINARY_LIST in config:
        for messagesensor in config[CONF_SENSOR_ACME_OT_BINARY_LIST]:
            sens = await binary_sensor.new_binary_sensor(messagesensor)
            cg.add(var.add_sensor_acme_binary(sens, messagesensor[CONF_SENSOR_ACME_OT_MESSAGE_ID], messagesensor[CONF_SENSOR_ACME_OT_VALUE_ON_REQUEST], messagesensor[CONF_SENSOR_ACME_OT_BINARY_BIT]))

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

def opentherm_component_schema():
    """Create a schema for a OpenTherm component.
    :return: The OpenTherm component schema, `extend` this in your config schema.
    """
    schema = {
        cv.GenerateID(CONF_OPENTHERM_ID): cv.use_id(OpenThermGW),
    }
    return cv.Schema(schema)