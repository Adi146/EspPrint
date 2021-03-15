#pragma once

#include "gcode_sensor.h"
#include "sd_gcode_sender.h"

namespace sensors{
  class GCodeEvents: public GCodeSensor, public CustomAPIDevice {
  private:
    std::string m_eventPrefix;
    std::string m_method;

    SDGCodeSender* m_sender;
  
  public:
    GCodeEvents(std::string eventPrefix, std::string method, SDGCodeSender* sender): 
      GCodeSensor(), 
      CustomAPIDevice(),
      m_eventPrefix(eventPrefix), 
      m_method(method),
      m_sender(sender) {
    }

    void handleLine(std::string& gcode) override {
      if (is_connected() && m_sender->getFilename() == "") {
        fire_homeassistant_event("esphome." + m_eventPrefix + "_gcode_event", {
          { "method", m_method },
          { "gcode", gcode }
        });
      }
    }
  };
}