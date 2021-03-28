#pragma once

#include "gcode_sender.h"
#include "gcode_sensor.h"
#include "FS.h"
#include "SD_MMC.h"
#include <regex>

using namespace esphome;
using namespace esphome::custom_component;
using namespace sensors;

#define get_sdSender(constructor) static_cast<storage::SDGCodeSender *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

namespace storage {
  class SDGCodeSender: public GCodeSender {
  private:
    File m_file;
    fs::FS m_fs;

    std::string readNextGCode();

  public:
    SDGCodeSender(UARTComponent *parent, int resendBufferSize, fs::FS &fs);

    void loop() override;

    void print(std::string filename);

    void stop();

    std::string getFilename() {
      return m_file.available() ? std::string(m_file.name()) : "";
    }

    fs::FS& getFS() {
      return m_fs;
    }
  };
}