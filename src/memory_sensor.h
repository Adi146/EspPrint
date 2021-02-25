#pragma once

#include "esphome.h"

class MemorySensor: public PollingComponent, public Sensor {
public:
  MemorySensor(): PollingComponent(1000) {}

  void update() override {
    publish_state(ESP.getFreeHeap());
  }
};