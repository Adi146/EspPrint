#pragma once

#include "esphome/core/component.h"
#include "FS.h"

using namespace esphome;

namespace storage {
  namespace filesystem {
    class Filesystem: public Component {
    public:
      virtual uint64_t getTotalSize() = 0;
      virtual uint64_t getUsedSize() = 0;

      virtual float get_setup_priority() const override {
        return setup_priority::HARDWARE;
      }
    };
  }

  extern filesystem::Filesystem* fsAdapter;
  extern fs::FS& fs;
}