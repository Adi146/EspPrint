#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/GCodeQueue/GCodeAnalyzer.h"
#include <regex>

using namespace esphome;
using namespace esphome::sensor;

namespace core {
  namespace analyzer {
    class M73_Sensor: public GCodeAnalyzer, public Component, public Sensor {
    private:
      std::regex m_M73Rgx = std::regex(R"(M73 P(\d+) R(\d+))");

      int m_regexGroupIndex;

    public:
      M73_Sensor(int regexGroupIndex):
        GCodeAnalyzer(),
        m_regexGroupIndex(regexGroupIndex) {
      }

      void handleLine(std::string& gcode, GCodeSource source) override {
        if (source != GCodeSource::SENDER) {
          return;
        }

        std::smatch match;
        if (std::regex_search(gcode, match, m_M73Rgx)) {
          publish_state(atoi(match[m_regexGroupIndex].str().c_str()));
        }
      }
    };
  }
}