#pragma once

#include "gcode_sensor.h"

namespace sensors{
  class ActionSensor: public GCodeSensor, public TextSensor {
  private:
    std::string m_actionStr = "//action:notification ";

  public:
    ActionSensor(): GCodeSensor(), TextSensor() {}

    bool handleLine(std::string& gcode) override {
      if (gcode.rfind(m_actionStr, 0) == 0) {
        publish_state(gcode.substr(m_actionStr.length()));
        return true;
      }

      return false;
    }
  };
}