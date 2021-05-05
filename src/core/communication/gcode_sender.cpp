#include "gcode_sender.h"

using namespace core::communication;

GCodeSender::GCodeSender(UARTComponent *parent, int resendBufferSize, GCodeQueue* analyzerQueue): 
  Component(),
  CustomAPIDevice(),
  UARTDevice(parent), 
  m_buffer(BUFFER_SIZE), 
  m_resendBuffer(resendBufferSize),
  m_analyzerQueue(analyzerQueue) {
}

void GCodeSender::setup() {
  register_service(&GCodeSender::sendGCode, "send_gcode", {"gcode"});

  Threading::setup(4 * 1024, configMAX_PRIORITIES - 1, 1);
}

void GCodeSender::threadLoop() {
  m_sendMutex.lock();

  if (m_resendLineNumber >= 0) {
    if (m_resendBuffer.empty()) {
      m_resendLineNumber = -1;
    }

    if (m_resendLineNumber == m_resendBuffer.getReadPtr()) {
      auto gcode = m_resendBuffer.peek();
      _sendGCode(gcode, m_resendLineNumber);
      m_resendLineNumber++;
    }
  } 
  else {
    m_bufferMutex.lock();
    while (!m_buffer.empty() && !m_resendBuffer.full()) {
      auto gcode = m_buffer.pop();
      m_resendBuffer.push(gcode);
      _sendGCode(gcode, m_resendBuffer.getWritePtr());
    }
    m_bufferMutex.unlock();
  }

  if (!m_resendBuffer.empty() && millis() - m_lastCommandTimestamp > 10000) {   
    ESP_LOGW("gcode_sender", "timeout for line %llu, fake ok", m_resendBuffer.getReadPtr());
    m_timeoutCounter++;
    ok(0, 0, m_resendBuffer.getReadPtr());
  }
  m_sendMutex.unlock();
}

void GCodeSender::_sendGCode(std::string gcode, uint64_t lineNumber) {
  char buffer[156] = "";
  snprintf(buffer, sizeof(buffer), "N%llu %s", lineNumber, gcode.c_str());

  int checksum = 0;
  for (auto i = 0; i < sizeof(buffer); i++) {
    checksum ^= buffer[i];
  }

  snprintf(buffer, sizeof(buffer), "%s*%d", buffer, checksum);

  write_str(buffer);
  write('\n');

  std::string strGCode(buffer);
  m_analyzerQueue->handleLine(strGCode, GCodeSource::SENDER);

  m_lastCommandTimestamp = millis();
}

void GCodeSender::sendGCode(std::string gcode) {
  m_bufferMutex.lock();
  m_buffer.push(gcode);
  m_bufferMutex.unlock();
}

void GCodeSender::reset() {
  m_sendMutex.lock();
  m_bufferMutex.lock();
  m_resendCounter = 0;
  m_timeoutCounter = 0;

  m_buffer.reset();
  m_resendBuffer.reset();
  _sendGCode("M110 N0", 0);
  m_bufferMutex.unlock();
  m_sendMutex.unlock();
}

void GCodeSender::handleOK(int plannerBuffer, int commandBuffer, uint64_t lineNumber) {
  m_sendMutex.lock();
  ok(plannerBuffer, commandBuffer, lineNumber);
  m_sendMutex.unlock();
}

void GCodeSender::ok(int plannerBuffer, int commandBuffer, uint64_t lineNumber) {
  m_lastCommandTimestamp = millis();

  if (lineNumber >= m_resendBuffer.getReadPtr() && lineNumber <= m_resendBuffer.getWritePtr()) {
    m_resendBuffer.setReadPtr(lineNumber + 1);
  } 
  else if(!m_resendBuffer.empty()) {
    m_resendBuffer.pop();
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