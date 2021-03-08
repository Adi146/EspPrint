#include "gcode_reader.h"

GCodeReader::GCodeReader(UARTComponent* parent, GCodeSender* sender): 
  UARTDevice(parent), 
  m_sender(sender),
  m_sensorBuffer(READER_SENSOR_BUFFER_SIZE){
}

void GCodeReader::setup() {
  Threading::setup();
}

void GCodeReader::loop() {
  while(!m_sensorBuffer.empty()) {
    std::string line = m_sensorBuffer.pop();

    ESP_LOGI("gcode_reader", "RECV: %s", line.c_str());

    for (auto sensor: m_sensors) {
      if (sensor->handleLine(line))
        break;
    } 
  }
}

bool GCodeReader::readLine(std::string* line) {
  for (int i = available(); i > 0; i--) {
    m_readBuffer[m_readBuffer_ptr] = read();
    if (m_readBuffer[m_readBuffer_ptr] == '\n' || m_readBuffer[m_readBuffer_ptr] >= 128) {
      m_readBuffer[m_readBuffer_ptr] = 0;
      m_readBuffer_ptr = 0;
      *line = std::string(m_readBuffer);
      return true;
    }

    m_readBuffer_ptr++;
  }

  return false;
}

bool GCodeReader::handleOK(std::string& line) {
  std::smatch match;
  if (std::regex_match(line, match, m_okRgx)) {
    int64_t lineNumber = match[2].matched ? atoi(match[2].str().c_str()) : -1;
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

bool GCodeReader::handleBusy(std::string& line) {
  if (line == m_busyStr) {
    m_sender->handleBusy();
    return true;
  }

  return false;
}

void GCodeReader::threadLoop() {
  std::string line;

  if (readLine(&line)) {
    handleOK(line) || handleBusy(line) || handleResend(line);

    if(!m_sensorBuffer.full())
      m_sensorBuffer.push(line);
  }
}