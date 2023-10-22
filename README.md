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
The gateway is an ESPHome external component. To use it, you only need to include it in your configuration file:

```yaml
external_components:
  - source: github://Reproduktor/esphome-openthermgw
    components: [ openthermgw ]
```
