#pragma once

#include "gcode_sender.h"
#include "FS.h"
#include <regex>

using namespace esphome::custom_component;

#define get_sdSender(constructor) static_cast<SDGCodeSender *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

class SDGCodeSender: public GCodeSender {
private:
  File file;
  fs::FS fs_;

  std::regex M73Rgx = std::regex(R"(^M73 P(\d+) R(\d+)$)");

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

  void handleM73(std::string& gcode) {
    std::smatch match;
    if (std::regex_match(gcode, match, M73Rgx)) {
      printProgress.publish_state(atoi(match[1].str().c_str()));
      remainingTime.publish_state(atoi(match[2].str().c_str()));
    }
  }

public:
  Sensor printProgress;
  Sensor remainingTime;

  SDGCodeSender(UARTComponent *parent, fs::FS &fs) : GCodeSender(parent), fs_(fs) {}

  void setup() override {}

  void loop() override {
    if (file.available() && !buffer.full()) {
      std::string gcode = readNextGCode();
      if (!gcode.empty()) {
        bufferGCode(gcode);

        handleM73(gcode);
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
};