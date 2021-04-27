#include "temperature_sensor.h"

using namespace core::analyzer;

GCodeClimate::GCodeClimate(GCodeSender* sender): 
Climate(),
m_sender(sender) {
}

void GCodeClimate::control(const ClimateCall& call) {
  int val;

  if (call.get_mode().has_value() && *call.get_mode() == CLIMATE_MODE_OFF) {
    val = 0;
  }
  else if (call.get_target_temperature().has_value()) {
    val = (int)*call.get_target_temperature();
  } 
  else {
    return;
  }

  char buffer[MAX_GCODE_LENGTH] = "";

  switch (m_token){
    case 'T':
      snprintf(buffer, sizeof(buffer), "M104 T%d S%d", m_index, val);
      break;   
    case 'B':
      snprintf(buffer, sizeof(buffer), "M140 S%d", val);
      break;
    case 'C':
      snprintf(buffer, sizeof(buffer), "M141 S%d", val);
      break;   
    default:
      return;
  }

  m_sender->sendGCode(std::string(buffer));
  publish_state();
}

ClimateTraits GCodeClimate::traits() {
  auto traits = ClimateTraits();
  traits.set_supports_current_temperature(true);
  return traits;
}

void GCodeClimate::update(float currentTemp, float targetTemp, char token, int index) {
  current_temperature = currentTemp;
  target_temperature = targetTemp;

  m_token = token;
  m_index = index;

  if (target_temperature == 0) {
    mode = CLIMATE_MODE_OFF;
  }
  else if(target_temperature > 0) {
    mode = CLIMATE_MODE_HEAT;
  }

  publish_state();
}

TemperatureSensor::TemperatureSensor(int numTemperatures, GCodeSender* sender) :
GCodeAnalyzer() {
  m_temperatures.reserve(numTemperatures);
  for(int i = 0; i < numTemperatures; i++) {
    m_temperatures.push_back(new GCodeClimate(sender));
  }
}

void TemperatureSensor::handleLine(std::string& gcode, GCodeSource source) {
  if (source != GCodeSource::READER) {
    return;
  }

  auto begin = std::sregex_iterator(gcode.begin(), gcode.end(), m_temperatureRgx);
  auto end = std::sregex_iterator();

  for (auto i = begin; i != end; i++) {
    std::smatch match = *i;
    int tempIndex = std::distance(begin, i);
    char token = match[1].str().c_str()[0];
    int toolIndex = atoi(match[2].str().c_str());

    float currentTemp = parse_float(match[3].str()).value_or(0.0f);
    float targetTemp = parse_float(match[5].str()).value_or(0.0f);

    m_temperatures[tempIndex]->update(currentTemp, targetTemp, token, toolIndex);
  }
}