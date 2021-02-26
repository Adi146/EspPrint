#pragma once

#include "gcode_sender.h"
#include "gcode_sensor.h"
#include "FS.h"
#include <regex>
#include <vector>

using namespace esphome::custom_component;
using namespace sensors;

#define get_sdSender(constructor) static_cast<SDGCodeSender *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

class SDGCodeSender: public GCodeSender {
private:
  File file;
  fs::FS fs_;

  std::vector<GCodeSensor*> m_sensors = std::vector<GCodeSensor*>();

  std::string readNextGCode() {
    char buffer[128] = "";
    for (int i = 0; i < sizeof(buffer) && file.available(); i++) {
      char c = file.read();
      switch (c) {
      case ';':
        file.readStringUntil('\n');
      case '\n':
        if (i > 1)
          return std::string(buffer);
        i = -1;
      default:
        buffer[i] = c;
        if (i < sizeof(buffer))
          buffer[i + 1] = 0;
      }
    }

    return buffer;
  }

public:
  SDGCodeSender(UARTComponent *parent, fs::FS &fs) : GCodeSender(parent), fs_(fs) {}

  void setup() override {}

  void loop() override {
    if (file.available() && !buffer.full()) {
      std::string gcode = readNextGCode();
      if (!gcode.empty()) {
        bufferGCode(gcode);

        for (auto sensor: m_sensors) {
          if (sensor->handleLine(gcode))
            break;
        }
      }
    }

    GCodeSender::loop();
  }

  void print(std::string filename) {
    ESP_LOGI("sdcard", "Start printing from SD-Card %s", filename.c_str());

    file = fs_.open(filename.c_str());
    if (!file) {
      ESP_LOGE("sdcard", "File does not exist: %s", filename.c_str());
      return;
    }

    buffer.reset();
    resendBuffer.reset();
    sendGCodeForce("M110 N0");
  }

  void stop() {
    file.close();

    sendGCode("M104 S0");
    sendGCode("M140 S0");
    sendGCode("G28 X Y");
    sendGCode("M84");
  }

  void addSensor(GCodeSensor* sensor) {
    m_sensors.push_back(sensor);
  }
};