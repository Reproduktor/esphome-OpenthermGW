CODEOWNERS = ["@reproduktor/esphome-openthermgw"]

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, binary_sensor, switch, number
from esphome.automation import maybe_simple_id
from esphome import pins
from esphome.const import *
from esphome.const import CONF_ID, ENTITY_CATEGORY_CONFIG, CONF_NAME
from esphome.core import coroutine_with_priority

CONF_MASTER_IN_PIN = "master_in_pin"
CONF_MASTER_OUT_PIN = "master_out_pin"
CONF_SLAVE_IN_PIN = "slave_in_pin"
CONF_SLAVE_OUT_PIN = "slave_out_pin"
CONF_OPENTHERM_ID = "opentherm_id"

opentherm_ns = cg.esphome_ns.namespace("openthermgw")
local_switch_ns = cg.esphome_ns.namespace("local_switch")

OpenThermGW = opentherm_ns.class_("OpenthermGW", cg.Component)

# OverrideNumber = opentherm_ns.class_(
#     "OverrideNumber", number.Number, cg.Component
# )
# OverrideNumberSwitch = opentherm_ns.class_(
#     "OverrideSwitch", switch.Switch, cg.Component
# )
# OverrideBinary = opentherm_ns.class_(
#     "OverrideBinary", switch.Switch, cg.Component
# )
OverrideBinarySwitch = opentherm_ns.class_(
    "OverrideBinarySwitch", switch.Switch, cg.Component
)

AUTO_LOAD = ['sensor', 'binary_sensor', 'switch']
MULTI_CONF = False

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
CONF_SENSOR_ACME_OT_BINARY_BIT = "bitindex"
CONF_SCHEMA_ACME_OT_BINARY = binary_sensor.binary_sensor_schema().extend(
    {
        cv.Required(CONF_SENSOR_ACME_OT_MESSAGE_ID): cv.positive_int,
        cv.Optional(CONF_SENSOR_ACME_OT_VALUE_ON_REQUEST, default='false'): cv.boolean,
        cv.Required(CONF_SENSOR_ACME_OT_BINARY_BIT): cv.int_range(1, 16), #1-16 bit index
    }
    )

CONF_SENSOR_ACME_OT_OVERRIDE_BINARY_SWITCH_LIST = "acme_opentherm_override_binary_switches"
CONF_SCHEMA_ACME_OT_OVERRIDE_BINARY_SWITCH = cv.maybe_simple_value(
    switch.switch_schema(
        OverrideBinarySwitch,
        entity_category=ENTITY_CATEGORY_CONFIG,
        default_restore_mode="RESTORE_DEFAULT_OFF",
        ).extend(
            {
                cv.Required(CONF_SENSOR_ACME_OT_MESSAGE_ID): cv.positive_int,
                cv.Optional(CONF_SENSOR_ACME_OT_VALUE_ON_REQUEST, default='false'): cv.boolean,
                cv.Required(CONF_SENSOR_ACME_OT_BINARY_BIT): cv.int_range(1, 16), #1-16 bit index
            }
        ),
        key=CONF_NAME,
    )

# CONF_SENSOR_ACME_OT_OVERRIDE_BINARY_LIST = "acme_opentherm_override_binaries"
# CONF_SCHEMA_ACME_OT_OVERRIDE_BINARY = switch.switch_schema(
#     OverrideBinary,
#     entity_category=ENTITY_CATEGORY_CONFIG,
#     default_restore_mode="RESTORE_DEFAULT_OFF",
#     ).extend(
#         {
#             cv.Required(CONF_SENSOR_ACME_OT_MESSAGE_ID): cv.positive_int,
#             cv.Optional(CONF_SENSOR_ACME_OT_VALUE_ON_REQUEST, default='false'): cv.boolean,
#             cv.Required(CONF_SENSOR_ACME_OT_BINARY_BIT): cv.int_range(1, 16), #1-16 bit index
#         }
#     )

# CONF_SENSOR_ACME_OT_OVERRIDE_BINARY_LIST = "acme_opentherm_override_binaries"
# CONF_SCHEMA_ACME_OT_OVERRIDE_BINARY = switch.switch_schema(
#     OverrideSwitch,
#     entity_category=ENTITY_CATEGORY_CONFIG,
#     default_restore_mode="RESTORE_DEFAULT_OFF",
#     )

# CONF_SENSOR_ACME_OT_OVERRIDE_NUMBER_LIST = "acme_opentherm_override_numbers"
# CONF_SCHEMA_ACME_OT_OVERRIDE_NUMBER = number.NUMBER_SCHEMA.extend(
#     {
#         OverrideSwitch,
#         entity_category=ENTITY_CATEGORY_CONFIG,
#         default_restore_mode="RESTORE_DEFAULT_OFF",
#     }

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThermGW),
        cv.Required(CONF_MASTER_IN_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_MASTER_OUT_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_SLAVE_IN_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_SLAVE_OUT_PIN): pins.internal_gpio_input_pin_number,

        cv.Optional(CONF_SENSOR_ACME_OT_LIST): cv.All(
            cv.ensure_list(CONF_SCHEMA_ACME_OT), cv.Length(min=1, max=200)
            ),
        cv.Optional(CONF_SENSOR_ACME_OT_BINARY_LIST): cv.All(
            cv.ensure_list(CONF_SCHEMA_ACME_OT_OVERRIDE_BINARY_SWITCH), cv.Length(min=1, max=200)
            ),
        cv.Optional(CONF_SENSOR_ACME_OT_OVERRIDE_BINARY_SWITCH_LIST): cv.All(
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
        for messagebinarysensor in config[CONF_SENSOR_ACME_OT_BINARY_LIST]:
            binarysens = await binary_sensor.new_binary_sensor(messagebinarysensor)
            cg.add(var.add_sensor_acme_binary(binarysens, messagebinarysensor[CONF_SENSOR_ACME_OT_MESSAGE_ID], messagebinarysensor[CONF_SENSOR_ACME_OT_VALUE_ON_REQUEST], messagebinarysensor[CONF_SENSOR_ACME_OT_BINARY_BIT]))


def opentherm_component_schema():
    """Create a schema for a OpenTherm component.
    :return: The OpenTherm component schema, `extend` this in your config schema.
    """
    schema = {
        cv.GenerateID(CONF_OPENTHERM_ID): cv.use_id(OpenThermGW),
    }
    return cv.Schema(schema)