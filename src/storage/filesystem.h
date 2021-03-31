#pragma once

#include "esphome.h"
#include "FS.h"
#include "file_reader.h"

using namespace esphome;
using namespace esphome::api;

namespace storage {
  struct Fileinfo {
    std::string path;
    size_t size;
  };

  class Filesystem: public Component, public CustomAPIDevice {
  private:
    fs::FS& m_fs;
    std::string m_eventPrefix;

    std::vector<Fileinfo> m_files;

    std::vector<Fileinfo> listDirectory(File file);

  public:
    Filesystem(FileReader* fileReader, std::string eventPrefix);

    void setup() override;

    void fireListEvent();
    void deleteFile(std::string);
  };
}