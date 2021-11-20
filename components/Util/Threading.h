#pragma once

#include "esphome/core/hal.h"

namespace debug {
  class StackSensor;
}

namespace util {
  class Threading {
  friend debug::StackSensor;
  public:
    virtual void setup(uint32_t stackSize, UBaseType_t priority, BaseType_t core) {
      xTaskCreatePinnedToCore(Threading::t, "threadLoop", stackSize, this, priority, &m_thread, core);
    }

    virtual void threadLoop() = 0;

    int m_delayMs = 1;

    uint32_t getStackWaterMark() {
      return uxTaskGetStackHighWaterMark(m_thread);
    }

  private:
    TaskHandle_t m_thread;

    static void t(void * param) {
      Threading* t = (Threading*) param;

      while(true) {
        t->threadLoop();
        esphome::delay(t->m_delayMs);
      }
    }
  };
}
