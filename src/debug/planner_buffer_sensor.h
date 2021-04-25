#pragma once

#include "esphome.h"
#include "communication/gcode_sender.h"

using namespace core::communication;

namespace debug {
  class PlannerBufferSensor: public Component, public Sensor {
  private:
    GCodeSender* m_sender;

  public:
    PlannerBufferSensor(GCodeSender* sender): m_sender(sender) {}

    void loop() override {
      auto val = m_sender->getFreePlannerBuffer();
      if (val != get_state()){
        publish_state(val);
      } 
    }
  };
}