#pragma once

#include "gcode_sender.h"
#include "gcode_sensor.h"
#include "FS.h"
#include <regex>
#include <vector>

using namespace esphome;
using namespace esphome::custom_component;
using namespace sensors;

#define get_sdSender(constructor) static_cast<SDGCodeSender *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

class SDGCodeSender: public GCodeSender {
private:
  File m_file;
  fs::FS m_fs;
  std::vector<GCodeSensor*> m_sensors = std::vector<GCodeSensor*>();

  std::string readNextGCode();

public:
  SDGCodeSender(UARTComponent *parent, fs::FS &fs);

  void loop() override;

  void print(std::string filename);

  void stop();

  void addSensor(GCodeSensor* sensor) {
    m_sensors.push_back(sensor);
  }

  std::string getFilename() {
    return m_file.available() ? std::string(m_file.name()) : "";
  }
};