#pragma once

#include "gcode_sender.h"
#include "fs_adapter.h"
#include "threading.h"

using namespace esphome;
using namespace esphome::custom_component;
using namespace esphome::api;

#define get_fileReader(constructor) static_cast<storage::FileReader *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

namespace storage {
  class FileReader: public Component, public CustomAPIDevice, public util::Threading {
  private:
    GCodeSender* m_sender;

    File m_file;

    std::vector<std::string> m_cancelGCodes;

    void readNextGCode(std::string& gcode);

  public:
    FileReader(GCodeSender* sender, std::vector<std::string> cancelGCodes);

    void setup() override;

    void threadLoop() override;

    void print(std::string filename);

    void stop();

    std::string getFilename() {
      return m_file.available() ? std::string(m_file.name()) : "";
    }
  };
}