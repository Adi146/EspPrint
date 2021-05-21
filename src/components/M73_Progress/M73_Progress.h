#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "gcode_analyzer.h"
#include <regex>

using namespace esphome;
using namespace esphome::sensor;

namespace core {
  namespace analyzer {
    class M73_Progress: public GCodeAnalyzer, public Component, public Sensor {
    private:
      std::regex m_M73Rgx = std::regex(R"(M73 P(\d+) R\d+)");

    public:
      M73_Progress(): GCodeAnalyzer() {}

      void handleLine(std::string& gcode, GCodeSource source) override {
        if (source != GCodeSource::SENDER) {
          return;
        }

        std::smatch match;
        if (std::regex_search(gcode, match, m_M73Rgx)) {
          publish_state(atoi(match[1].str().c_str()));
        }
      }
    };
  }
}