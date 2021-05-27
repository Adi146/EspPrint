#pragma once

#include "esphome/core/component.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/GCodeSender/GCodeSender.h"
#include "esphome/components/Filesystem/Filesystem.h"
#include "esphome/components/Util/Threading.h"

using namespace esphome;
using namespace esphome::api;
using namespace core::communication;

namespace storage {
  class GCodeFileReader: public Component, public CustomAPIDevice, public util::Threading {
  private:
    GCodeSender* m_sender;

    File m_file;

    std::string m_tmpGCode;
    std::vector<std::string> m_cancelGCodes;

    void readNextGCode(std::string& gcode);

  public:
    GCodeFileReader(GCodeSender* sender, std::vector<std::string> cancelGCodes);

    void setup() override;

    void threadLoop() override;

    void print(std::string filename);

    void stop();

    std::string getFilename() {
      return m_file.available() ? std::string(m_file.name()) : "";
    }
  };
}