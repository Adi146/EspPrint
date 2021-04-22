#pragma once

#include "esphome.h"
#include "file_reader.h"

namespace storage{
  class FileSensor: public PollingComponent, public TextSensor {
  private:
    storage::FileReader* m_fileReader;

  public:
    FileSensor(FileReader* fileReader): 
      PollingComponent(10000), 
      m_fileReader(fileReader) {  
    }

    void update() override {
      auto val = m_fileReader->getFilename();
      if (val == "") {
        publish_state("None");
      } 
      else {
        publish_state(val);
      }
    }
  };
}