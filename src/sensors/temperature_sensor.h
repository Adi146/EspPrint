#pragma once

#include "gcode_sensor.h"

namespace sensors{
  class TemperatureSensor: public GCodeSensor {
  private:
    std::regex m_temperatureRgx = std::regex(R"(([BTA](\d*)):\s*([-+]?[0-9]*\.?[0-9]+)(\s*\/\s*([-+]?[0-9]*\.?[0-9]+)))");

  public:
    std::vector<Sensor*> m_temperatures;

    TemperatureSensor(int numTemperatures): GCodeSensor() {
      m_temperatures.reserve(numTemperatures * 2);
      for(int i = 0; i < numTemperatures * 2; i++) {
        m_temperatures.push_back(new Sensor());
      }
    }

    void handleLine(std::string& gcode) override {
      auto begin = std::sregex_iterator(gcode.begin(), gcode.end(), m_temperatureRgx);
      auto end = std::sregex_iterator();
  
      for (auto i = begin; i != end; i++) {
        std::smatch match = *i;
        int tempIndex = std::distance(begin, i) * 2;
  
        float currentTemp = parse_float(match[3].str()).value_or(0.0f);
        float targetTemp = parse_float(match[5].str()).value_or(0.0f);
  
        m_temperatures[tempIndex]->publish_state(currentTemp);
        m_temperatures[tempIndex + 1]->publish_state(targetTemp);
      }
    }
  };
}