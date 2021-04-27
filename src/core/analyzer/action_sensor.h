#pragma once

#include "esphome.h"
#include "gcode_analyzer.h"

namespace core {
  namespace analyzer {
    class ActionSensor: public GCodeAnalyzer, public Component, public TextSensor {
    private:
      std::string m_actionStr = "//action:notification ";

    public:
      ActionSensor(): GCodeAnalyzer(), TextSensor() {}

      void handleLine(std::string& gcode, GCodeSource source) override {
        if (source != GCodeSource::READER) {
          return;
        }

        if (gcode.rfind(m_actionStr, 0) == 0) {
          publish_state(gcode.substr(m_actionStr.length()));
        }
      }
    };
  }
}