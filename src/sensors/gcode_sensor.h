#pragma once

#include "esphome.h"

using namespace esphome;

namespace sensors {
  class GCodeSensor: public Component {
  public:
    GCodeSensor(): Component() {}

    virtual void handleLine(std::string& gcode) = 0;
  };
}