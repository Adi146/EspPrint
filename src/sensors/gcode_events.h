#pragma once

#include "esphome.h"
#include "gcode_sensor.h"
#include "ring_buffer.h"
#include "ArduinoJson.h"

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
      DynamicJsonDocument doc(1024);

      auto i = 0;
      while (!m_buffer.empty()) {
        auto elem = m_buffer.pop();
        doc[i]["gcode"] = elem.gcode;
        doc[i]["source"] = elem.source == GCodeSource::READER ? "RECV" : "SEND";
        i++;
      }

      std::string tmp;
      serializeJson(doc, tmp);

      if (i != 0) {
        fire_homeassistant_event("esphome." + m_eventPrefix + "_gcode_event", {
          {"gcodes", tmp}
        });
      }
    }
  };
}