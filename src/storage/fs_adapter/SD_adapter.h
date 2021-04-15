#pragma once

#include "SD.h"
#include "fs_adapter.h"


namespace storage {
  namespace fs_adapter {
    class SDAdapter: public FSAdapter {
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

  fs_adapter::FSAdapter* fsAdapter = new fs_adapter::SDAdapter();
  fs::FS& fs = SD;
}