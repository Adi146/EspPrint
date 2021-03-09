#include "gcode_sender.h"

GCodeSender::GCodeSender(UARTComponent *parent, int resendBufferSize): 
  UARTDevice(parent), 
  m_buffer(BUFFER_SIZE), 
  m_resendBuffer(resendBufferSize),
  m_sensorBuffer(SENDER_SENSOR_BUFFER_SIZE) {
}

void GCodeSender::setup() {
  Threading::setup();
}

void GCodeSender::loop() {
  while(!m_sensorBuffer.empty()) {
    std::string line = m_sensorBuffer.pop();

    ESP_LOGI("gcode_sender", "SEND: %s", line.c_str());

    for (auto sensor: m_sensors) {
      if (sensor->handleLine(line))
        break;
    } 
  }
}

void GCodeSender::threadLoop() {
  m_sendMutex.lock();

  if (m_resendLineNumber >= 0) {
    if (m_resendBuffer.empty()) {
      m_resendLineNumber = -1;
    }

    if (m_resendLineNumber == m_resendBuffer.getReadPtr()) {
      auto gcode = m_resendBuffer.peek();
      sendGCodeForce(gcode, m_resendLineNumber);
      m_resendLineNumber++;
    }
  } 
  else if (!m_buffer.empty() && !m_resendBuffer.full()) {
    sendGCode(m_buffer.pop());
  }

  if (!m_resendBuffer.empty() && millis() - m_lastCommandTimestamp > 10000) {   
    ESP_LOGW("gcode_sender", "timeout for line %llu, fake ok", m_resendBuffer.getReadPtr());
    m_timeoutCounter++;
    ok(0, 0, m_resendBuffer.getReadPtr());
  }
  m_sendMutex.unlock();
}

void GCodeSender::sendGCodeForce(std::string gcode, uint64_t lineNumber) {
  char buffer[156] = "";
  snprintf(buffer, sizeof(buffer), "N%llu %s", lineNumber, gcode.c_str());

  int checksum = 0;
  for (auto i = 0; i < sizeof(buffer); i++) {
    checksum ^= buffer[i];
  }

  snprintf(buffer, sizeof(buffer), "%s*%d", buffer, checksum);

  write_str(buffer);
  write('\n');

  if(!m_sensorBuffer.full()) {
    std::string strGCode(buffer);
    m_sensorBuffer.push(strGCode);
  }

  m_lastCommandTimestamp = millis();
}

void GCodeSender::sendGCode(std::string gcode) {
  m_resendBuffer.push(gcode);
  sendGCodeForce(gcode, m_resendBuffer.getWritePtr());
}

void GCodeSender::reset() {
  m_sendMutex.lock();
  m_resendCounter = 0;
  m_timeoutCounter = 0;

  m_buffer.reset();
  m_resendBuffer.reset();
  sendGCodeForce("M110 N0", 0);
  m_sendMutex.unlock();
}

void GCodeSender::handleOK(int plannerBuffer, int commandBuffer, int64_t lineNumber) {
  m_sendMutex.lock();
  ok(plannerBuffer, commandBuffer, lineNumber);
  m_sendMutex.unlock();
}

void GCodeSender::ok(int plannerBuffer, int commandBuffer, int64_t lineNumber) {
  m_lastCommandTimestamp = millis();

  if (lineNumber >= m_resendBuffer.getReadPtr() && lineNumber <= m_resendBuffer.getWritePtr()) {
    m_resendBuffer.setReadPtr(lineNumber + 1);
  }
  
  m_okGCodeBuffer = commandBuffer;
  m_okPlannerBuffer = plannerBuffer;
}

void GCodeSender::handleResend(uint64_t lineNumber) {
  m_sendMutex.lock();
  m_lastCommandTimestamp = millis();
  if (m_resendBuffer.getWritePtr() >= lineNumber) {
    if (m_resendBuffer.getReadPtr() > lineNumber) {
      m_resendBuffer.setReadPtr(lineNumber);
    }
    m_resendLineNumber = lineNumber;
    m_resendCounter++;
  }
  m_sendMutex.unlock();
}

void GCodeSender::handleBusy() {
  m_sendMutex.lock();
  m_lastCommandTimestamp = millis();
  m_sendMutex.unlock();
}