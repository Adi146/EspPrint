#pragma once

#include "gcode_sensor.h"
#include "sd_gcode_sender.h"
#include "ring_buffer.h"

#define EVENTS_BUFFER_SIZE 100

namespace sensors{
  struct BufferElement {
    std::string gcode;
    GCodeSource source;
  };

  class GCodeEvents: public GCodeSensor, public PollingComponent, public CustomAPIDevice {
  private:
    std::string m_eventPrefix;

    util::RingBuffer<BufferElement> m_buffer;
  
  public:
    GCodeEvents(std::string eventPrefix): 
      GCodeSensor(),
      PollingComponent(1000),
      CustomAPIDevice(),
      m_eventPrefix(eventPrefix), 
      m_buffer(EVENTS_BUFFER_SIZE){
    }

    void handleLine(std::string& gcode, GCodeSource source) override {
      m_buffer.push({gcode, source});

      if (m_buffer.full()) {
        update();
      }
    }

    void update() override {
      std::string tmp = "";
      while (!m_buffer.empty()) {
        auto elem = m_buffer.pop();
        tmp += elem.source == GCodeSource::READER ? "RECV: " : "SEND: ";
        tmp += elem.gcode;
        tmp += '\n';
      }

      if (tmp != "") {
        fire_homeassistant_event("esphome." + m_eventPrefix + "_gcode_event", {
          {"gcode", tmp}
        });
      }
    }
  };
}