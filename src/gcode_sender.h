
#pragma once

#include "esphome.h"
#include "ring_buffer.h"
#include "threading.h"
#include "gcode_sensor.h"
#include <string>
#include <mutex>
#include <vector>

#define BUFFER_SIZE 50
#define SENDER_SENSOR_BUFFER_SIZE 20
#define MAX_GCODE_LENGTH 256

using namespace sensors;
using namespace esphome;
using namespace esphome::uart;
using namespace esphome::custom_component;

#define get_sender(constructor) static_cast<GCodeSender *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

namespace storage {
  class FileReader;
}

class GCodeSender : public Component, public UARTDevice, public util::Threading {
friend storage::FileReader;

protected:
  util::RingBuffer<std::string> m_buffer;
  util::RingBuffer<std::string> m_resendBuffer;

  int m_resendCounter = 0;
  int m_timeoutCounter = 0;
  int m_okPlannerBuffer = 0;
  int m_okGCodeBuffer = 0;

  int64_t m_resendLineNumber = -1;

  std::vector<GCodeSensor*> m_sensors = std::vector<GCodeSensor*>();
  util::RingBuffer<std::string> m_sensorBuffer;

  uint32_t m_lastCommandTimestamp;

  std::mutex m_sendMutex;
  std::mutex m_bufferMutex;

  void ok(int plannerBuffer, int commandBuffer, uint64_t lineNumber);

  void _sendGCode(std::string gcode, uint64_t lineNumber);

public:
  GCodeSender(UARTComponent *parent, int resendBufferSize);

  void setup() override;

  void loop() override;

  void threadLoop() override;

  void sendGCode(std::string gcode);

  void reset();

  void handleOK(int plannerBuffer, int commandBuffer, uint64_t lineNumber);

  void handleResend(uint64_t lineNumber);

  void handleBusy();

  int getResendCounter() {
    return m_resendCounter;
  }

  int getTimeoutCounter() {
    return m_timeoutCounter;
  }

  int getFreePlannerBuffer() {
    return m_okPlannerBuffer;
  }

  int getFreeGCodeBuffer() {
    return m_okGCodeBuffer;
  }

  void addSensor(GCodeSensor* sensor) {
    m_sensors.push_back(sensor);
  }
};