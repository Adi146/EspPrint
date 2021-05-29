#pragma once

#include "esphome/core/component.h"
#include "esphome/components/logger/logger.h"
#include "esphome/components/GCodeQueue/GCodeAnalyzer.h"
#include <queue>
#include <mutex>

using namespace esphome;

#define QUEUE_SIZE 200

#define get_analyzerQueue(constructor) static_cast<GCodeQueue *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

namespace core {
  namespace analyzer
  {
    struct QueueEntry {
      std::string gcode;
      GCodeSource source;
    };

    class GCodeQueue : public GCodeAnalyzer, public Component {
    private:
      std::mutex m_queueMutex;
      std::queue<QueueEntry> m_queue = std::queue<QueueEntry>();

      std::vector<GCodeAnalyzer*> m_sensors = std::vector<GCodeAnalyzer*>();

    public:
      GCodeQueue();

      virtual void handleLine(std::string& gcode, GCodeSource source) override;

      virtual float get_setup_priority() const override {
        return setup_priority::LATE;
      }

      void loop() override;

      void addSensor(GCodeAnalyzer* sensor) {
        m_sensors.push_back(sensor);
      }
    };
  }  
}