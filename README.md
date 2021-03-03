# EspPrint

[![Compile](https://github.com/Adi146/EspPrint/actions/workflows/compile.yml/badge.svg)](https://github.com/EspPrint/actions/actions)

EspPrint is a ESP32 firmware to control 3D-Printers with GCode. 
It is based on [ESPHome](https://esphome.io/) and therefore integrates well in [Home Assistant](https://www.home-assistant.io/).

:exclamation::exclamation::exclamation:

This project is still under development. There is no stable release yet!

I am no responsible for any possible damage! Use it on your own risk!

:exclamation::exclamation::exclamation:

## Project Goal
The goal of this project is to create an alternative to [Octoprint](https://octoprint.org/) which runs on a Raspberry Pi.
ESP32s are way cheaper than Raspberry Pis and should provide enough computational power to handle simple GCode communication.
Most functions that are solved with plugins in Octoprint like switching plugs and led strips can be handled with automations in Home Assistant.

## Firmware Requirements
To compensate the lower power of the ESP compared to the Rasperry Pi, the firmware needs a few more informations from the printer.
Here are the firmware features in the Marlin configuration which have to be enabled:
* ADVANCED_OK
* AUTO_REPORT_TEMPERATURES
* HOST_ACTION_COMMANDS
* BUFSIZE 32
* RX_BUFFER_SIZE 2048

## Installation
1. [Install Python and ESPHome](https://esphome.io/guides/getting_started_command_line.html)
2. Flash your ESP32
```
esphome espprint.yaml run
```
3. Use free UART-Pins of you printers mainboard to connect the ESP32. 
   The ESP needs more power than a typical printer mainboard can supply with the 5V pins. Therefore you should use a Buck Converter.
4. Add the device to Home Assistant


## Tested Environments
### Printer Boards
* MKS Robin Nano V1.2
### Printer Firmware
* Marlin 2.0
### ESPs
* ESP32 Cam Module
