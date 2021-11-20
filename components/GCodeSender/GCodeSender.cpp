#include "GCodeSender.h"

using namespace core::communication;

GCodeSender::GCodeSender(GCodeQueue* analyzerQueue): 
  Component(),
  CustomAPIDevice(),
  UARTDevice(),
  m_analyzerQueue(analyzerQueue) {
}

void GCodeSender::setup() {
  register_service(&GCodeSender::sendGCode, "send_gcode", {"gcode"});

  Threading::setup(4 * 1024, configMAX_PRIORITIES - 1, 1);
}

void GCodeSender::threadLoop() {
  if (!m_inputBuffer.empty() && m_sentLineNumber - m_processedLineNumber < m_printerGCodeBufferSize) {
    auto gcode = m_inputBuffer.front();
    m_resendBuffer.push_back(gcode);
    _sendGCode(gcode, m_sentLineNumber++);
    m_inputBuffer.pop();
  }

  // clock on ESP32 is not very precise.
  // sometimes millis() returns a value smaller than m_lastCommandTimestamp so we got to check for overflows.
  uint32_t delta = millis() - m_lastCommandTimestamp;
  if (delta > 10000 && delta < 200000 && m_sentLineNumber - m_processedLineNumber > 0) {   
    ESP_LOGW("gcode_sender", "timeout (%u) for line %llu, fake ok", delta, m_processedLineNumber);
    m_timeoutCounter++;
    ok(0, 0, 0, true);
  }
}

void GCodeSender::_sendGCode(std::string gcode, uint64_t lineNumber) {
  char buffer[MAX_GCODE_LENGTH] = "";
  snprintf(buffer, sizeof(buffer), "N%llu %s", lineNumber, gcode.c_str());

  int checksum = 0;
  for (auto i = 0; i < sizeof(buffer); i++) {
    checksum ^= buffer[i];
  }

  snprintf(buffer, sizeof(buffer), "%s*%d", buffer, checksum);

  write_str(buffer);
  write('\n');

  handleBusy();

  std::string strGCode(buffer);
  m_analyzerQueue->handleLine(strGCode, GCodeSource::SENDER);
}

bool GCodeSender::sendGCode(std::string gcode, int timeout) {
  // skip queue for emergency GCodes
  for (auto it: EMERGENCY_GCODE) {
    if (gcode.rfind(it, 0) == 0) {
      m_resendBuffer.push_back(gcode);
      _sendGCode(gcode, m_sentLineNumber++);
      return true;
    }
  }

  m_inputBufferMutex.lock();
  // wait until we got space in buffer or we timeout
  while(m_inputBuffer.size() > INPUT_BUFFER_SIZE) {
    if(timeout-- == 0){
      m_inputBufferMutex.unlock();
      return false;
    }
    delay(1);
  }

  m_inputBuffer.push(gcode);
  m_inputBufferMutex.unlock();
  return true;
}

void GCodeSender::reset() {
  m_resendCounter = 0;
  m_timeoutCounter = 0;

  m_sentLineNumber = 0;
  m_processedLineNumber = 0;

  m_inputBuffer = std::queue<std::string>();
  sendGCode("M110 N0", 0);
}

void GCodeSender::handleOK(int plannerBuffer, int commandBuffer, uint64_t lineNumber) {
  ok(plannerBuffer, commandBuffer, lineNumber, false);
}

void GCodeSender::ok(int plannerBuffer, int commandBuffer, uint64_t lineNumber, bool timeout) {
  handleBusy();

  if (lineNumber == 0) {
    lineNumber = m_processedLineNumber;
  }

  for(; m_processedLineNumber <= lineNumber; m_processedLineNumber++) {
    if(!timeout) {
      m_resendBuffer.pop_front();
    }
  }
  
  m_okGCodeBuffer = commandBuffer;
  m_okPlannerBuffer = plannerBuffer;
}

void GCodeSender::handleResend(uint64_t lineNumber) {
  m_resendCounter++;

  for(; lineNumber < m_sentLineNumber; lineNumber++) {
    _sendGCode(m_resendBuffer[lineNumber - m_processedLineNumber], lineNumber);
  }
}

void GCodeSender::handleBusy() {
  m_lastCommandTimestamp = millis();
}