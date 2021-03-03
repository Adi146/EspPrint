
#pragma once

#include "esphome.h"
#include "ring_buffer.h"
#include "threading.h"
#include "gcode_sensor.h"
#include <string>
#include <chrono>
#include <vector>

#define BUFFER_SIZE 50
#define RESEND_BUFFER_SIZE 50
#define RESERVED_PRINTER_GCODE_BUFFER 5
#define SENDER_SENSOR_BUFFER_SIZE 20

using namespace sensors;
using namespace esphome;
using namespace esphome::uart;
using namespace esphome::custom_component;

#define get_sender(constructor) static_cast<GCodeSender *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

class GCodeSender : public Component, public UARTDevice, public util::Threading {
protected:
  util::RingBuffer<std::string> m_buffer;
  util::RingBuffer<std::string> m_resendBuffer;

  int m_resendCounter = 0;
  bool m_resend = false;

  int m_curCommandBufferSize = 1;
  int m_curPlannerBufferSize = 1; 
  int m_maxCommandBufferSize = 1;
  int m_maxPlannerBufferSize = 1;

  std::vector<GCodeSensor*> m_sensors = std::vector<GCodeSensor*>();
  util::RingBuffer<std::string> m_sensorBuffer;

  void addLineNumberAndChecksum(std::string& gcode);

public:
  GCodeSender(UARTComponent *parent);

  void setup() override;

  void loop() override;

  void threadLoop() override;

  void sendGCodeForce(std::string gcode);

  void sendGCode(std::string gcode);

  bool bufferGCode(std::string& gcode);

  void handleOK(int plannerBuffer, int commandBuffer, int64_t lineNumber);

  void handleResend(uint64_t lineNumber);

  int getResendCounter() {
    return m_resendCounter;
  }

  void addSensor(GCodeSensor* sensor) {
    m_sensors.push_back(sensor);
  }
};