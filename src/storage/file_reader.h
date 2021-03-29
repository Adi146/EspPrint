#pragma once

#include "gcode_sender.h"
#include "FS.h"
#include "SD_MMC.h"

using namespace esphome;
using namespace esphome::custom_component;

#define get_fileReader(constructor) static_cast<storage::FileReader *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

namespace storage {
  class FileReader: public Component{
  private:
    GCodeSender* m_sender;

    File m_file;
    fs::FS m_fs;

    std::string readNextGCode();

  public:
    FileReader(GCodeSender* sender, fs::FS &fs);

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