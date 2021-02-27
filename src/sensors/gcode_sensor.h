#pragma once

#include "esphome.h"

using namespace esphome;

namespace sensors {
  class GCodeSensor: public Component {
  public:
    GCodeSensor(): Component() {}

    virtual bool handleLine(std::string& gcode) = 0;
  };
}