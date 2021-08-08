# EspPrint

[![Compile](https://github.com/Adi146/EspPrint/actions/workflows/compile.yml/badge.svg)](https://github.com/Adi146/EspPrint/actions)

These are Custom Componenets for [ESPHome](https://esphome.io/) to control 3D-Printers with GCode over a ESP32.
There are also some Lovelace Cards for [Home Assistant](https://www.home-assistant.io/) provided for better integration.

:exclamation::exclamation::exclamation:

This project is still under development. There is no stable release yet!

I am no responsible for any possible damage! Use it on your own risk!

:exclamation::exclamation::exclamation:

## Project Goal
The goal of this project is to create an alternative to [Octoprint](https://octoprint.org/) which runs on a Raspberry Pi.
ESP32s are way cheaper than Raspberry Pis and should provide enough computational power to handle simple GCode communication.
Most functions that are solved with plugins in Octoprint like switching plugs and led strips can be handled with automations in Home Assistant.

## Firmware Requirements
Here are the firmware features in the Marlin which have to be enabled:
* AUTO_REPORT_TEMPERATURES

Optional:
* ADVANCED_OK
* HOST_ACTION_COMMANDS

## Installation
1. [Install Python and ESPHome](https://esphome.io/guides/getting_started_command_line.html)
2. Change configuration in espprint.yaml, secrets.yaml and config folder
3. Flash your ESP32
```
esphome espprint.yaml run
```
3. Use free UART-Pins of you printers mainboard to connect the ESP32. 
   The ESP needs more power than a typical printer mainboard can supply with the 5V pins therefore you should use a Buck Converter.
4. Add the ESP to Home Assistant as an ESPHome device
5. Add the custom cards in the cards directory to your Home Assistant Dashboard

## Slicer configuration
At the moment only Prusa Slicer is supported. The setup is the same as with octoprint except that the API Key is not required.

## Tested Environments
The following Boards and ESPs are tested but other Hardware should work too.
### Printer Boards
* MKS Robin Nano V1.2
* BTT SKR 1.4
* FYSETC S6
### Printer Firmware
* Marlin 2.0
### ESPs
* AI-Thinker ESP32 Cam Module
* ESP32 D1 Mini
