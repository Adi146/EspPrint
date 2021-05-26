#pragma once

#include "SD.h"
#include "esphome/components/Filesystem/Filesystem.h"

namespace storage {
  namespace filesystem {
    class SD_Filesystem: public Filesystem {
    public:
      void setup() override {
        SD.begin();
      }

      uint64_t getTotalSize() override {
        return SD.totalBytes();
      }

      uint64_t getUsedSize() override {
        return SD.usedBytes();
      }
    };
  }

  filesystem::Filesystem* fsAdapter = new filesystem::SD_Filesystem();
  fs::FS& fs = SD;
}