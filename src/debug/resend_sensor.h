#pragma once

#include "communication/gcode_sender.h"

using namespace core::communication;

namespace debug {
  class ResendSensor: public PollingComponent, public Sensor {
  private:
    GCodeSender* m_sender;

  public:
    ResendSensor(GCodeSender* sender): PollingComponent(5000), m_sender(sender) {}

    void update() override {
      publish_state(m_sender->getResendCounter());
    }
  };
}