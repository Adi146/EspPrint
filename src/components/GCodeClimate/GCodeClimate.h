#pragma once

#include "esphome/components/climate/climate.h"
#include "esphome/components/GCodeSender/GCodeSender.h"
#include "esphome/components/GCodeQueue/GCodeAnalyzer.h"
#include <regex>

using namespace esphome;
using namespace esphome::climate;
using namespace core::communication;

namespace core {
  namespace analyzer {
    enum ClimateToken: char {
      TOOL = 'T',
      BED = 'B',
      CHAMBER = 'A'
    };

    class GCodeClimate: public Climate, public GCodeAnalyzer, public Component {
    private:
      ClimateToken m_token;
      int m_index;

      GCodeSender* m_sender;

      std::regex m_temperatureRgx = std::regex(R"(([BTA](\d*)):\s*([-+]?[0-9]*\.?[0-9]+)(\s*\/\s*([-+]?[0-9]*\.?[0-9]+)))");

    public:
      GCodeClimate(GCodeSender* sender, ClimateToken token, int index = 0);

      void control(const ClimateCall& call) override;

      ClimateTraits traits() override;

      void update(float currentTemp, float targetTemp);

      void handleLine(std::string& gcode, GCodeSource source) override;
    };
  }
}