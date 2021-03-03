#pragma once

namespace util {
  class Threading {
  public:
    virtual void setup() {
      xTaskCreate(Threading::t, "readLoop", 16 * 1024, this, 1, &m_thread);
    }

    virtual void threadLoop() = 0;

    int m_delayMs = 10;

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
