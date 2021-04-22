#pragma once

namespace sensors {
  class StackSensor;
}

namespace util {
  class Threading {
  friend sensors::StackSensor;
  public:
    virtual void setup(uint32_t stackSize, UBaseType_t priority, BaseType_t core) {
      xTaskCreatePinnedToCore(Threading::t, "threadLoop", stackSize, this, priority, &m_thread, core);
    }

    virtual void threadLoop() = 0;

    int m_delayMs = 1;

  private:
    TaskHandle_t m_thread;

    static void t(void * param) {
      Threading* t = (Threading*) param;

      while(true) {
        t->threadLoop();
        delay(t->m_delayMs);
      }
    }
  };
}
