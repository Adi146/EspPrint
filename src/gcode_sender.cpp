#include "gcode_sender.h"

GCodeSender::GCodeSender(UARTComponent *parent):  UARTDevice(parent), m_buffer(BUFFER_SIZE), m_resendBuffer(RESEND_BUFFER_SIZE) {
}

std::string GCodeSender::addLineNumberAndChecksum(std::string gcode) {
  uint64_t lineNumber = m_resendBuffer.getWritePtr() + 1; 
  gcode = "N" + to_string(lineNumber) + " " + gcode;

  int checksum = 0;
  for (auto it = gcode.cbegin(); it != gcode.cend(); it++)
    checksum ^= *it;

  return gcode + "*" + to_string(checksum);
}

void GCodeSender::loop() {
  high_resolution_clock::time_point now = high_resolution_clock::now();
  auto ms = duration_cast<milliseconds>(now - m_lastCommandTimestamp).count();

  if (m_curCommandBufferSize > RESERVED_PRINTER_GCODE_BUFFER && ms > m_throttleDelayMS) {
    if (m_resend) {
      sendGCodeForce(m_resendBuffer.pop());
      m_resend = !m_resendBuffer.empty();
    }
    else if(!m_buffer.empty()) {
      sendGCode(m_buffer.pop());
    }

    m_lastCommandTimestamp = now;
    int linesWithoutResend = m_resendBuffer.getWritePtr() - m_resendBuffer.getReadPtr();
    if(linesWithoutResend > 0 && linesWithoutResend % 1000 == 0 && m_throttleDelayMS > MIN_DELAY_MS){
      m_throttleDelayMS--;
      ESP_LOGI("gcode_sender", "communication seems to be stable, trying to decrease throttle to %d ms", m_throttleDelayMS);
    }
  }
}

void GCodeSender::sendGCodeForce(std::string gcode) {
  ESP_LOGI("gcode_sender", "SEND: %s", gcode.c_str());
  write_str((gcode + "\n").c_str());
  m_curCommandBufferSize--;
}

void GCodeSender::sendGCode(std::string gcode) {
  gcode = addLineNumberAndChecksum(gcode);
  m_resendBuffer.push(gcode);
  sendGCodeForce(gcode);
}

bool GCodeSender::bufferGCode(std::string& gcode) {
  if (!m_buffer.full()) {
    m_buffer.push(gcode);
    return true;
  }

  return false;
}

void GCodeSender::handleOK(int plannerBuffer, int commandBuffer, uint64_t lineNumber) {
  m_curPlannerBufferSize = plannerBuffer;
  m_curCommandBufferSize = commandBuffer;

  if (plannerBuffer > m_maxPlannerBufferSize)
    m_maxPlannerBufferSize = plannerBuffer;
  if (commandBuffer > m_maxCommandBufferSize)
    m_maxCommandBufferSize = commandBuffer;
}

void GCodeSender::handleResend(uint64_t lineNumber) {
  m_resendBuffer.setReadPtr(lineNumber);
  m_resend = true;
  m_resendCounter++;
  ESP_LOGW("gcode_sender", "resend requested for line %llu", lineNumber);
  if (m_throttleDelayMS < MAX_DELAY_MS) {
    m_throttleDelayMS++;
    ESP_LOGI("gcode_sender", "increase throttle to %d ms", m_throttleDelayMS);
  }
}