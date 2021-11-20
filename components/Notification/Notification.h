#pragma once

#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/GCodeQueue/GCodeAnalyzer.h"

using namespace esphome;
using namespace esphome::text_sensor;
using namespace core::analyzer;

namespace action {
  class Notification: public GCodeAnalyzer, public Component, public TextSensor {
  private:
    std::string m_notificationStr = "//action:notification ";

  public:
    Notification(): GCodeAnalyzer(), TextSensor() {}

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