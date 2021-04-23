#pragma once

#include "esphome.h"
#include "fs_adapter.h"
#include "ArduinoJson.h"

using namespace esphome;
using namespace esphome::api;

#define get_fileanalyzer(constructor) static_cast<storage::analyzer::Fileanalyzer*>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

namespace storage {
  namespace analyzer {
    struct Fileinfo {
      std::string path;
      size_t size;
      time_t lastWrite;

      virtual void fillJsonObject(JsonObject& obj);
    };

    class Fileanalyzer: public Component, public CustomAPIDevice {
    private:
      std::string m_eventPrefix;

      std::vector<Fileinfo> m_files;

    protected:
      virtual Fileinfo analyze(fs::File& file);

    public:
      Fileanalyzer(std::string eventPrefix);

      void setup() override;

      void fireListEvent();

      void addFile(fs::File& file);
      void addFile(std::string path);

      void deleteFile(std::string path);
      void removeFromList(std::string path);
    };
  }
}