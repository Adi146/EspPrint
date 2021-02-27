#include "gcode_reader.h"

GCodeReader::GCodeReader(UARTComponent* parent, GCodeSender* sender): UARTDevice(parent), m_sender(sender) {
}

std::string GCodeReader::readLine() {
  for (int i = available(); i > 0; i--) {
    m_readBuffer[m_readBuffer_ptr] = read();
    if (m_readBuffer[m_readBuffer_ptr] == '\n' || m_readBuffer[m_readBuffer_ptr] >= 128) {
      m_readBuffer[m_readBuffer_ptr] = 0;
      m_readBuffer_ptr = 0;
      return std::string(m_readBuffer);
    }

    m_readBuffer_ptr++;
  }

  return std::string();
}

bool GCodeReader::handleOK(std::string& line) {
  std::smatch match;

  if (std::regex_search(line, match, m_okRgx)) {
    int lineNumber = match[2].matched ? atoi(match[2].str().c_str()) : -1;
    int plannerBuffer = atoi(match[4].str().c_str());
    int commandBuffer = atoi(match[6].str().c_str());

    m_sender->handleOK(plannerBuffer, commandBuffer, lineNumber);
    return true;
  }
    
  return false;
}

bool GCodeReader::handleResend(std::string& line) {
  std::smatch match;
  if (std::regex_search(line, match, m_resendRgx)) {
    m_sender->handleResend(atoi(match[1].str().c_str()));
    return true;
  }

  return false;
}

void GCodeReader::loop() {
  std::string line = readLine();
  if(line.length() > 0) {
    ESP_LOGI("gcode_reader", "RECV: %s", line.c_str());

    if (handleOK(line))
      return;
      
    if (handleResend(line))
      return;

    for (auto sensor: m_sensors) {
      if (sensor->handleLine(line))
        break;
    }
  }  
}