#pragma once

#include "esphome.h"
#include "FS.h"

using namespace esphome;

namespace storage {
  namespace fs_adapter {
    class FSAdapter: public Component {
    public:
      virtual uint64_t getTotalSize() = 0;
      virtual uint64_t getUsedSize() = 0;

      virtual float get_setup_priority() const override {
        return setup_priority::HARDWARE;
      }
    };
  }

  extern fs_adapter::FSAdapter* fsAdapter;
  extern fs::FS& fs;
}