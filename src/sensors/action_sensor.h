#pragma once

#include "esphome.h"
#include "gcode_sensor.h"

namespace sensors{
  class ActionSensor: public GCodeSensor, public Component, public TextSensor {
  private:
    std::string m_actionStr = "//action:notification ";

  public:
    ActionSensor(): GCodeSensor(), TextSensor() {}

    void handleLine(std::string& gcode, GCodeSource source) override {
      if (gcode.rfind(m_actionStr, 0) == 0) {
        publish_state(gcode.substr(m_actionStr.length()));
      }
    }
  };
}