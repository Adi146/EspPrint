#pragma once

#include "esphome.h"
#include "gcode_sender.h"
#include "ring_buffer.h"
#include "threading.h"
#include <regex>
#include <string>

#define READER_SENSOR_BUFFER_SIZE 20

using namespace sensors;
using namespace esphome;
using namespace esphome::uart;
using namespace esphome::custom_component;

#define get_reader(constructor) static_cast<GCodeReader *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

class GCodeReader : public Component, public UARTDevice, public util::Threading {
private:
  std::regex m_okRgx = std::regex(R"(^ok (N(\d+) )?(P(\d+)) (B(\d+))$)");
  std::regex m_resendRgx = std::regex(R"(Resend: (\d+))");

  std::vector<GCodeSensor*> m_sensors = std::vector<GCodeSensor*>();

  GCodeSender* m_sender;

  char m_readBuffer[256];
  int m_readBuffer_ptr = 0;

  util::RingBuffer<std::string> m_sensorBuffer;

protected:
  bool handleOK(std::string& line);

  bool handleResend(std::string& line);

public:
  GCodeReader(UARTComponent* parent, GCodeSender* sender);

  void setup() override;

  void loop() override;

  void threadLoop() override;

  bool readLine(std::string* line);

  void addSensor(GCodeSensor* sensor) {
    m_sensors.push_back(sensor);
  }
};