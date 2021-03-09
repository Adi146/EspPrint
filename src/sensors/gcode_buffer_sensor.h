#pragma once

#include "esphome.h"
#include "gcode_sender.h"

namespace sensors{
  class GCodeBufferSensor: public Component, public Sensor {
  private:
    GCodeSender* m_sender;

  public:
    GCodeBufferSensor(GCodeSender* sender): m_sender(sender) {}

    void loop() override {
      auto val = m_sender->getFreeGCodeBuffer();
      if (val != get_state()){
        publish_state(val);
      }      
    }
  };
}