#pragma once

#include "esphome.h"
#include "SD.h"
#include "SD_MMC.h"
#include "FS.h"
#include "ArduinoJson.h"

using namespace esphome;
using namespace esphome::api;

#define get_filesystem(constructor) static_cast<storage::Filesystem*>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

namespace storage {
  struct Fileinfo {
    std::string path;
    size_t size;
    time_t lastWrite;

    virtual void fillJsonObject(JsonObject& obj);
  };

  class Filesystem: public Component, public CustomAPIDevice {
  private:
    fs::FS& m_fs;
    std::string m_eventPrefix;

    std::vector<Fileinfo> m_files;

  protected:
    virtual Fileinfo analyze(fs::File& file);

  public:
    Filesystem(fs::FS& fs, std::string eventPrefix);

    void setup() override;

    void fireListEvent();
    void addFile(fs::File& file);
    void deleteFile(std::string);
  };
}