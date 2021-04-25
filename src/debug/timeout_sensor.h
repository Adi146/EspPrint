#pragma once

#include "communication/gcode_sender.h"

using namespace core::communication;

namespace debug {
  class TimeoutSensor: public PollingComponent, public Sensor {
  private:
    GCodeSender* m_sender;

  public:
    TimeoutSensor(GCodeSender* sender): PollingComponent(5000), m_sender(sender) {}

    void update() override {
      publish_state(m_sender->getTimeoutCounter());
    }
  };
}