#pragma once

#include "esphome.h"

namespace sensors {

class MemorySensor: public PollingComponent, public Sensor {
public:
  MemorySensor(): PollingComponent(1000) {}

  void update() override {
    publish_state(ESP.getFreeHeap());
  }
};
}