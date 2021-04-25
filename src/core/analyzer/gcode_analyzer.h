#pragma once

namespace core {
  namespace analyzer {
    enum GCodeSource {
      SENDER,
      READER
    };

    class GCodeAnalyzer {
    public:
      GCodeAnalyzer() {}

      virtual void handleLine(std::string& gcode, GCodeSource source) = 0;
    };
  }
}