# OpenthermGW for ESPHome / Home assistant
 
(as External component, by Reproduktor)

## Welcome!

I have two main requirements to solve:
1. I want to monitor what my QAA73 thermostat & Geminox THRi-DC boiler are up to
2. I want to be able to override some settings at occasion. For example, I want to control the hot water circulating pump on my own.


For some time, I've been using this [Opentherm Arduino shield from Jiří Praus](https://www.tindie.com/products/jiripraus/opentherm-gateway-arduino-shield/). I had it connected with an ESP8266-based board and using [Jiří's Arduino library](https://github.com/jpraus/arduino-opentherm) I added some MQTT communication to report the values and show them in my Home Assistant. I added OTA, so it was a practically usable solution. Once I was able to read the values, I let the project sleep and did not get to extend it for overriding the values.

When the time finally came, I wished I could somehow port this to ESPHome. The ecosystem gives you lots for free, native Home Assistant API support is seamless. And here goes this project. It is still only in its beginning, please create an Issue if you observe a problem, or want the functionality extended. If you get me a coffee, I may be more willing to do something about it 😊

[<img src="assorted/bmc_qr.png" width="150" height="150">](https://www.buymeacoffee.com/reproduktor)

A code cleanup is needed - noted.

## Acknowledgement
Ihor Myealnik's [Opentherm library](https://github.com/ihormelnyk/opentherm_library) is used by this component.

## Configuration

### Use the component

The gateway is an ESPHome external component. To use it, you only need to include it in your configuration file:

```yaml
external_components:
  - source: github://Reproduktor/esphome-openthermgw
    components: [ openthermgw ]
```

### Hardware configuration
You need to configure the pins, on which the Opentherm gateway is connected. Please note - `master` is the thermostat end, `slave` is the boiler end.

```yaml
openthermgw:
  master_in_pin: 19
  master_out_pin: 17
  slave_in_pin: 18
  slave_out_pin: 16
```

### Sensors - preface
I was trying to achieve such a configurability, that the component code does not need to be changed to just observe another message, or find out the details about the request and response messages. As a result, you are able to expose any message as a sensor, using the type decoding you specify. The same message ID can be used in multiple sensors, thus you can expose a sensor with different encoding (e.g., HB and LB as two different sensors). Binary sensors are handled separately, as they create a different kind of sensor - binary sensor.

### Adding numeric sensors

For the numeric sensors, you can create a list like this:

```yaml
  acme_opentherm_sensor_list:
    - name: "ACME Control setpoint"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 1
      value_on_request: false
      value_type: 2
    - name: "ACME Control setpoint 2"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 8
      value_on_request: false
      value_type: 2
    - name: "ACME Room setpoint"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 16
      value_on_request: false
      value_type: 2
    - name: "ACME Relative modulation level"
      device_class: "signal_strength"
      accuracy_decimals: 0
      unit_of_measurement: "%"
      message_id: 17
      value_on_request: false
      value_type: 2
```

#### Configuration variables

Sensor variables are inherited from ESPHome [Sensor component](https://esphome.io/components/sensor/index.html), plus:

- **message_id** (*Required*, positive int): Opentherm Message ID to capture in the sensor
- **value_type** (*Optional*, positive int range 0-7, default 0): Type of the value to retrieve from the Opentherm message. The types supported are:
  - **0** *(u16)* unsigned 16 bit integer
  - **1** *(s16)* signed 16 bit integer
  - **2** *(f16)* 16 bit float
  - **3** *(u8LB)* unsigned 8 bit integer in the lower byte of the message data
  - **4** *(u8HB)* unsigned 8 bit integer in the higher byte of the message data
  - **5** *(s8LB)* signed 8 bit integer in the lower byte of the message data
  - **6** *(s8HB)* signed 8 bit integer in the higher byte of the message data
  - **7** Request or response code of the Opentherm message. The value is directly read from the message. Possible values are:
    - Master-To-Slave (request)
      - 0 READ-DATA
      - 1 WRITE-DATA
      - 2 INVALID-DATA
    - Slave-To-Master (response)
      - 4 READ-ACK
      - 5 WRITE-ACK
      - 6 DATA-INVALID
      - 7 UNKNOWN-DATAID
- **value_on_request** (*Optional*, boolean, default `False`): **Only `False` is supported at the moment. `True` does not have any effect.** If `false`, the value is read from the slave (boiler) response message. If `true`, the value is read from the master (thermostat) request message.

### Adding binary sensors

Binary sensors are added like this:

```yaml
  acme_opentherm_binary_sensors:
    - name: "ACME Boiler fault"
      message_id: 0
      value_on_request: false
      bitindex: 1
    - name: "ACME Boiler CH mode"
      message_id: 0
      value_on_request: false
      bitindex: 2
    - name: "ACME Boiler DHW mode"
      message_id: 0
      value_on_request: false
      bitindex: 3
    - name: "ACME Boiler flame status"
      message_id: 0
      value_on_request: false
      bitindex: 4
```

#### Configuration variables

Sensor variables are inherited from ESPHome [Binary sensor component](https://esphome.io/components/binary_sensor/), plus:

- **message_id** (*Required*, positive int): Opentherm Message ID to capture in the sensor
- **bitindex** (*Required*, positive int range 1-16): The bitindex from the right (lsb) of the message data.
- **value_on_request** (*Optional*, boolean, default `False`): **Only `False` is supported at the moment. `True` does not have any effect.** If `false`, the value is read from the slave (boiler) response message. If `true`, the value is read from the master (thermostat) request message.

### Overriding binary sensors

You can add an override switch like this:

```yaml
  acme_opentherm_override_binary_switches:
    - name: "OT override DHW circulating pump"
      message_id: 129
      value_on_request: true
      bitindex: 4
      acme_opentherm_override_binary_value:
        name: "OT override DHW circulating pump with"
```

This example creates two switches: `OT override DHW circulating pump` is the switch which controls, if the value in the given message is overriden, and the `OT override DHW circulating pump with` switch specifies the value to override the data with. *(Note that this example is for Geminox THRi boiler, the message ID is proprietary and not according to Opentherm standard.)*

#### Configuration variables

For every message yyou wish to override, configure an independent switch to control, if the overriding is on or off. Switch variables are inherited from ESPHome [Switch component](https://esphome.io/components/switch/), plus:

- **message_id** (*Required*, positive int): Opentherm Message ID to capture in the sensor
- **bitindex** (*Required*, positive int range 1-16): The bitindex from the right (lsb) of the message data.
- **value_on_request** (*Optional*, boolean, default `True`): **Only `True` is supported at the moment. `False` does not have any effect.** If `false`, the value is overriden in the slave (boiler) response message. If `true`, the value is overriden in the master (thermostat) request message.
- **acme_opentherm_override_binary_value** (*Required*, Switch): Secondary switch to control the state, to which the overriding should happen.


# Complete configuration
```yaml
external_components:
  - source: github://Reproduktor/esphome-openthermgw
    components: [ openthermgw ]

openthermgw:
  master_in_pin: 19
  master_out_pin: 17
  slave_in_pin: 18
  slave_out_pin: 16
  acme_opentherm_sensor_list:
    - name: "ACME Control setpoint"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 1
      value_on_request: false
      value_type: 2
    - name: "ACME ASF flags / OEM Fault code"
      message_id: 5
      value_on_request: false
      value_type: 0
    - name: "ACME ASF flags / OEM Fault code response"
      message_id: 5
      value_on_request: false
      value_type: 7
    - name: "ACME Control setpoint 2"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 8
      value_on_request: false
      value_type: 2
    - name: "ACME OEM diagnostic code"
      message_id: 115
      value_on_request: false
      value_type: 0
    - name: "ACME OEM diagnostic code response"
      message_id: 115
      value_on_request: false
      value_type: 7

    - name: "ACME Room setpoint"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 16
      value_on_request: false
      value_type: 2
    - name: "ACME Relative modulation level"
      device_class: "signal_strength"
      accuracy_decimals: 0
      unit_of_measurement: "%"
      message_id: 17
      value_on_request: false
      value_type: 2
    - name: "ACME CH water pressure"
      device_class: "pressure"
      accuracy_decimals: 2
      unit_of_measurement: "bar"
      message_id: 18
      value_on_request: false
      value_type: 2

    - name: "ACME Room setpoint CH2"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 23
      value_on_request: false
      value_type: 2
    - name: "ACME Room temperature"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 24
      value_on_request: false
      value_type: 2
    - name: "ACME Boiler water temperature"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 25
      value_on_request: false
      value_type: 2
    - name: "ACME DHW temperature"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 26
      value_on_request: false
      value_type: 2
    - name: "ACME Outside temperature"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 27
      value_on_request: false
      value_type: 2
    - name: "ACME Return water temperature"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 28
      value_on_request: false
      value_type: 2
    - name: "ACME Flow temperature CH2"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 31
      value_on_request: false
      value_type: 2
    - name: "ACME Exhaust temperature"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 33
      value_on_request: false
      value_type: 1

    - name: "ACME DHW setpoint"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 56
      value_on_request: false
      value_type: 2

    - name: "ACME Max CH water setpoint"
      device_class: "temperature"
      accuracy_decimals: 1
      unit_of_measurement: "°C"
      message_id: 57
      value_on_request: false
      value_type: 2

  acme_opentherm_binary_sensors:
    - name: "ACME Boiler fault"
      message_id: 0
      value_on_request: false
      bitindex: 1
    - name: "ACME Boiler CH mode"
      message_id: 0
      value_on_request: false
      bitindex: 2
    - name: "ACME Boiler DHW mode"
      message_id: 0
      value_on_request: false
      bitindex: 3
    - name: "ACME Boiler flame status"
      message_id: 0
      value_on_request: false
      bitindex: 4
    - name: "ACME Boiler CH2 mode"
      message_id: 0
      value_on_request: false
      bitindex: 6
    - name: "ACME Boiler diagnostic indication"
      message_id: 0
      value_on_request: false
      bitindex: 7

    - name: "ACME Control CH enable"
      message_id: 0
      value_on_request: false
      bitindex: 9
    - name: "ACME Control DHW enable"
      message_id: 0
      value_on_request: false
      bitindex: 10
    - name: "ACME Control CH2 enable"
      message_id: 0
      value_on_request: false
      bitindex: 13

  acme_opentherm_override_binary_switches:
    - name: "OT override DHW circulating pump"
      message_id: 129
      value_on_request: true
      bitindex: 4
      acme_opentherm_override_binary_value:
        name: "OT override DHW circulating pump with"
```