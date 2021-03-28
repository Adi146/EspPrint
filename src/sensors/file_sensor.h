#pragma once

#include "esphome.h"
#include "sd_gcode_sender.h"

namespace sensors{
  class FileSensor: public PollingComponent, public TextSensor {
  private:
    storage::SDGCodeSender* m_sender;

  public:
    FileSensor(storage::SDGCodeSender* sender): 
      PollingComponent(10000), 
      m_sender(sender) {  
    }

    void update() override {
      auto val = m_sender->getFilename();
      if (val == "") {
        publish_state("None");
      } 
      else {
        publish_state(m_sender->getFilename());
      }
    }
  };
}