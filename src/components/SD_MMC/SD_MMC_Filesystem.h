#pragma once

#include "SD_MMC.h"
#include "esphome/components/Filesystem/Filesystem.h"


namespace storage {
  namespace filesystem {
    class SD_MMC_Filesystem: public Filesystem {
    public:
      void setup() override {
        SD_MMC.begin();
      }

      uint64_t getTotalSize() override {
        return SD_MMC.totalBytes();
      }

      uint64_t getUsedSize() override {
        return SD_MMC.usedBytes();
      }
    };
  }

  filesystem::Filesystem* fsAdapter = new filesystem::SD_MMC_Filesystem();
  fs::FS& fs = SD_MMC;
}