#pragma once

namespace sensors {
  enum GCodeSource {
    SENDER,
    READER
  };

  class GCodeSensor {
  public:
    GCodeSensor() {}

    virtual void handleLine(std::string& gcode, GCodeSource source) = 0;
  };
}