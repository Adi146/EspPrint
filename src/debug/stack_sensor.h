#pragma once

#include "esphome.h"
#include "threading.h"

namespace debug {
  class StackSensor: public PollingComponent, public Sensor {
  public:
    StackSensor(util::Threading* thread):
      PollingComponent(1000),
      m_thread(thread) {}

    void update() override {
      publish_state(uxTaskGetStackHighWaterMark(m_thread->m_thread));
    }
  private:
    util::Threading* m_thread;
  };
}