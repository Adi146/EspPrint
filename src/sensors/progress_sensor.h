#pragma once

#include "gcode_sensor.h"

namespace sensors{
  class ProgressSensor: public GCodeSensor {
  private:
    std::regex m_M73Rgx = std::regex(R"(M73 P(\d+) R(\d+))");

  public:
    Sensor m_printProgress;
    Sensor m_remainingTime;

    ProgressSensor(): GCodeSensor() {}

    void handleLine(std::string& gcode) override {
      std::smatch match;
      if (std::regex_search(gcode, match, m_M73Rgx)) {
        m_printProgress.publish_state(atoi(match[1].str().c_str()));
        m_remainingTime.publish_state(atoi(match[2].str().c_str()));
      }
    }
  };
}