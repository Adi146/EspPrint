#pragma once

#include "esphome.h"
#include "gcode_analyzer.h"

namespace core {
  namespace analyzer {
    class ProgressSensor: public GCodeAnalyzer, public Component {
    private:
      std::regex m_M73Rgx = std::regex(R"(M73 P(\d+) R(\d+))");

    public:
      Sensor m_printProgress;
      Sensor m_remainingTime;

      ProgressSensor(): GCodeAnalyzer() {}

      void handleLine(std::string& gcode, GCodeSource source) override {
        std::smatch match;
        if (std::regex_search(gcode, match, m_M73Rgx)) {
          m_printProgress.publish_state(atoi(match[1].str().c_str()));
          m_remainingTime.publish_state(atoi(match[2].str().c_str()));
        }
      }
    };
  }
}