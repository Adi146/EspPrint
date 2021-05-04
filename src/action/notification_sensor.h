#pragma once

#include "esphome.h"
#include "analyzer/gcode_analyzer.h"

using namespace core::analyzer;

namespace action {
  class NotificationSensor: public GCodeAnalyzer, public Component, public TextSensor {
  private:
    std::string m_notificationStr = "//action:notification ";

  public:
    NotificationSensor(): GCodeAnalyzer(), TextSensor() {}

    void handleLine(std::string& gcode, GCodeSource source) override {
      if (source != GCodeSource::READER) {
        return;
      }

      if (gcode.rfind(m_notificationStr, 0) == 0) {
        publish_state(gcode.substr(m_notificationStr.length()));
      }
    }
  };
}