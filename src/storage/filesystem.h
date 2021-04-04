#pragma once

#include "esphome.h"
#include "FS.h"
#include "ArduinoJson.h"

using namespace esphome;
using namespace esphome::api;

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

    std::vector<Fileinfo> listDirectory(fs::File& directory);

  protected:
    virtual Fileinfo analyze(fs::File& file);

  public:
    Filesystem(fs::FS& fs, std::string eventPrefix);

    void setup() override;

    void fireListEvent();
    void deleteFile(std::string);
  };
}