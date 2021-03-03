#include "gcode_sender.h"

GCodeSender::GCodeSender(UARTComponent *parent): 
  UARTDevice(parent), 
  m_buffer(BUFFER_SIZE), 
  m_resendBuffer(RESEND_BUFFER_SIZE),
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

void GCodeSender::addLineNumberAndChecksum(std::string& gcode) {
  uint64_t lineNumber = m_resendBuffer.getWritePtr() + 1; 
  gcode = "N" + to_string(lineNumber) + " " + gcode;

  int checksum = 0;
  for (auto it = gcode.cbegin(); it != gcode.cend(); it++)
    checksum ^= *it;

  gcode = gcode + "*" + to_string(checksum);
}

void GCodeSender::threadLoop() {
  if (m_resend) {
    if(m_curCommandBufferSize == m_maxCommandBufferSize) {
      sendGCodeForce(m_resendBuffer.peek());
    }
  } 
  else {
    if (!m_buffer.empty() && !m_resendBuffer.full()) {
      sendGCode(m_buffer.pop());
    }
  }
}

void GCodeSender::sendGCodeForce(std::string gcode) {
  write_str((gcode + "\n").c_str());
  m_curCommandBufferSize--;

  if(!m_sensorBuffer.full())
    m_sensorBuffer.push(gcode);
}

void GCodeSender::sendGCode(std::string gcode) {
  addLineNumberAndChecksum(gcode);
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

void GCodeSender::handleOK(int plannerBuffer, int commandBuffer, int64_t lineNumber) {
  if (lineNumber >= 0) {
    m_resendBuffer.setReadPtr(lineNumber + 1);
    if (m_resend)
      m_resend = !m_resendBuffer.empty();
  }

  m_curPlannerBufferSize = plannerBuffer;
  m_curCommandBufferSize = commandBuffer;

  if (plannerBuffer > m_maxPlannerBufferSize)
    m_maxPlannerBufferSize = plannerBuffer;
  if (commandBuffer > m_maxCommandBufferSize){
    m_maxCommandBufferSize = commandBuffer;
    m_resendBuffer = util::RingBuffer<std::string>(commandBuffer - RESERVED_PRINTER_GCODE_BUFFER);
  }
        
}

void GCodeSender::handleResend(uint64_t lineNumber) {
  m_resendBuffer.setReadPtr(lineNumber);
  m_resend = true;
  m_resendCounter++;
}