#include "gcode_queue.h"

using namespace core::analyzer;

GCodeQueue::GCodeQueue() :
  GCodeAnalyzer() {
}

void GCodeQueue::handleLine(std::string& gcode, GCodeSource source) {
  m_queueMutex.lock();
  m_queue.push({gcode, source});
  m_queueMutex.unlock();
}

void GCodeQueue::loop() {
  while(!m_queue.empty()) {
    auto entry = m_queue.front();
    ESP_LOGI("queue", "%s: %s", entry.source == GCodeSource::READER ? "READ" : "SEND", entry.gcode.c_str());
    
    for (auto sensor: m_sensors) {      
      sensor->handleLine(entry.gcode, entry.source);
    }
    m_queue.pop();
  }
}