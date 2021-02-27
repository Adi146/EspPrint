
#pragma once

#include "esphome.h"
#include "ring_buffer.h"
#include "gcode_sensor.h"
#include <string>
#include <chrono>
#include <vector>

#define BUFFER_SIZE 50
#define RESEND_BUFFER_SIZE 50
#define MIN_DELAY_MS 10
#define MAX_DELAY_MS 20
#define RESERVED_PRINTER_GCODE_BUFFER 5

using namespace sensors;
using namespace esphome;
using namespace esphome::uart;
using namespace esphome::custom_component;
using namespace std::chrono;

#define get_sender(constructor) static_cast<GCodeSender *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

class GCodeSender : public Component, public UARTDevice {
protected:
  RingBuffer<std::string> m_buffer;
  RingBuffer<std::string> m_resendBuffer;

  int m_resendCounter = 0;
  bool m_resend = false;
  int m_throttleDelayMS = MIN_DELAY_MS;

  int m_curCommandBufferSize = 1;
  int m_curPlannerBufferSize = 1; 
  int m_maxCommandBufferSize = 1;
  int m_maxPlannerBufferSize = 1;

  high_resolution_clock::time_point m_lastCommandTimestamp = high_resolution_clock::now();

  std::vector<GCodeSensor*> m_sensors = std::vector<GCodeSensor*>();

  std::string addLineNumberAndChecksum(std::string gcode);

public:
  GCodeSender(UARTComponent *parent);

  void loop() override;

  void sendGCodeForce(std::string gcode);

  void sendGCode(std::string gcode);

  bool bufferGCode(std::string& gcode);

  void handleOK(int plannerBuffer, int commandBuffer, uint64_t lineNumber);

  void handleResend(uint64_t lineNumber);

  int getResendCounter() {
    return m_resendCounter;
  }

  void addSensor(GCodeSensor* sensor) {
    m_sensors.push_back(sensor);
  }
};