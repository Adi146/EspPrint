#pragma once

#include "esphome.h"
#include "sd_gcode_sender.h"

namespace sensors{
  class FileSensor: public PollingComponent, public TextSensor {
  private:
    SDGCodeSender* m_sender;

  public:
    FileSensor(SDGCodeSender* sender): PollingComponent(10000), m_sender(sender) {}

    void update() override {
      publish_state(m_sender->getFilename());
    }
  };
}