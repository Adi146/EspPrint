#pragma once

#include "SD_MMC.h"
#include "fs_adapter.h"


namespace storage {
  namespace fs_adapter {
    class SD_MMCAdapter: public FSAdapter {
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

  fs_adapter::FSAdapter* fsAdapter = new fs_adapter::SD_MMCAdapter();
  fs::FS& fs = SD_MMC;
}