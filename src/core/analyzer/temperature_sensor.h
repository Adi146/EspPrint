#pragma once

#include "gcode_analyzer.h"
#include "esphome/components/GCodeSender/GCodeSender.h"
#include <regex>

using namespace esphome;
using namespace esphome::climate;
using namespace core::communication;

namespace core {
  namespace analyzer {
    class GCodeClimate: public Climate {
    private:
      char m_token;
      int m_index;

      GCodeSender* m_sender;

    public:
      GCodeClimate(GCodeSender* sender);

      void control(const ClimateCall& call) override;

      ClimateTraits traits() override;

      void update(float currentTemp, float targetTemp, char token, int index);
    };

    class TemperatureSensor: public GCodeAnalyzer {
    private:
      std::regex m_temperatureRgx = std::regex(R"(([BTA](\d*)):\s*([-+]?[0-9]*\.?[0-9]+)(\s*\/\s*([-+]?[0-9]*\.?[0-9]+)))");

      std::vector<GCodeClimate*> m_temperatures;

    public:
      TemperatureSensor(int numTemperatures, GCodeSender* sender);

      void handleLine(std::string& gcode, GCodeSource source) override;

      std::vector<Climate*> getClimates() {
        return std::vector<Climate*>(m_temperatures.begin(), m_temperatures.end());
      }
    };
  }
}