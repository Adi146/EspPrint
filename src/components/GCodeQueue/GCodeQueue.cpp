#include "GCodeQueue.h"

using namespace core::analyzer;

GCodeQueue::GCodeQueue() :
  GCodeAnalyzer() {
}

void GCodeQueue::handleLine(std::string& gcode, GCodeSource source) {
  m_queueMutex.lock();
  if (m_queue.size() < QUEUE_SIZE) {
    m_queue.push({gcode, source});
  }  
  m_queueMutex.unlock();
}

void GCodeQueue::loop() {
  while(!m_queue.empty()) {
    auto entry = m_queue.front();
    ESP_LOGI("GCodeQueue", "%s: %s", entry.source == GCodeSource::READER ? "READ" : "SEND", entry.gcode.c_str());
    
    for (auto sensor: m_sensors) {      
      sensor->handleLine(entry.gcode, entry.source);
    }
    m_queue.pop();
  }
}