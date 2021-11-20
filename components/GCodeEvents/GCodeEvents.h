#pragma once

#include "esphome/core/component.h"
#include "esphome/core/application.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/GCodeSender/GCodeSender.h"
#include "esphome/components/GCodeQueue/GCodeAnalyzer.h"
#include "esphome/components/Util/RingBuffer.h"
#include "ArduinoJson.h"

#define EVENTS_BUFFER_SIZE 50

using namespace esphome;
using namespace esphome::api;

namespace core {
  namespace analyzer {
    struct BufferElement {
      std::string gcode;
      GCodeSource source;
    };

    class GCodeEvents: public GCodeAnalyzer, public PollingComponent, public CustomAPIDevice {
    private:
      util::RingBuffer<BufferElement> m_buffer;

    public:
      GCodeEvents(): 
        GCodeAnalyzer(),
        PollingComponent(1000),
        CustomAPIDevice(),
        m_buffer(EVENTS_BUFFER_SIZE){
      }

      void handleLine(std::string& gcode, GCodeSource source) override {
        m_buffer.push({gcode, source});

        if (m_buffer.full()) {
          update();
        }
      }

      void update() override {
        DynamicJsonDocument doc(MAX_GCODE_LENGTH * m_buffer.fillLevel());

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
          fire_homeassistant_event("esphome.espprint_gcodes", {
            { "device_id", App.get_name() },
            { "gcodes", tmp }
          });
        }
      }
    };
  }
}