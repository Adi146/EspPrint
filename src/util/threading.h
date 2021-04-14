#pragma once

namespace util {
  class Threading {
  public:
    virtual void setup() {
      xTaskCreatePinnedToCore(Threading::t, "threadLoop", 8 * 1024, this, configMAX_PRIORITIES - 1, &m_thread, 0);
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
