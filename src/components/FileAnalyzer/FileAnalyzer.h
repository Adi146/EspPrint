#pragma once

#include "esphome/core/component.h"
#include "esphome/core/application.h"
#include "esphome/components/logger/logger.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/Filesystem/Filesystem.h"
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

  class FileAnalyzer: public Component, public CustomAPIDevice {
  private:
    std::vector<Fileinfo> m_files;

  protected:
    virtual Fileinfo analyze(fs::File& file);

  public:
    FileAnalyzer();

    void setup() override;

    void fireListEvent();

    void addFile(fs::File& file);
    void addFile(std::string path);

    void deleteFile(std::string path);
    void removeFromList(std::string path);
  };
}