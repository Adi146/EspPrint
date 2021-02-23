#include "gcode_reader.h"

#define get_readerSensors(constructor) static_cast<GCodeReaderSensors *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

class GCodeReaderSensors: public GCodeReader {
private:
  std::regex temperatureRgx = std::regex(R"(([BTA](\d*)):\s*([-+]?[0-9]*\.?[0-9]+)(\s*\/\s*([-+]?[0-9]*\.?[0-9]+)))");
  std::string actionStr = "//action:notification ";

  bool updateTemperatureSensors(std::string line) {
    auto begin = std::sregex_iterator(line.begin(), line.end(), temperatureRgx);
    auto end = std::sregex_iterator();

    for (auto i = begin; i != end; i++) {
      std::smatch match = *i;
      int tempIndex = std::distance(begin, i) * 2;

      float currentTemp = parse_float(match[3].str()).value_or(0.0f);
      float targetTemp = parse_float(match[5].str()).value_or(0.0f);

      temperatures[tempIndex]->publish_state(currentTemp);
      temperatures[tempIndex + 1]->publish_state(targetTemp);
    }

    return begin != end;
  }

  bool handleAction(std::string line) {
    if (line.rfind(actionStr, 0) == 0) {
      lastAction.publish_state(line.substr(actionStr.length()));
      return true;
    }

    return false;
  }

protected:
  virtual bool processLine(std::string& line) override {
    if (GCodeReader::processLine(line))
      return true;
    
    if (updateTemperatureSensors(line))
      return true;

    if (handleAction(line))
      return true;

    return false;
  }

public:
  GCodeReaderSensors(UARTComponent* parent, GCodeSender* sender, int numTemperatures): 
  GCodeReader(parent, sender) {
    temperatures.reserve(numTemperatures * 2);
    for(int i = 0; i < numTemperatures * 2; i++) {
      temperatures.push_back(new Sensor());
    }
  }

  std::vector<Sensor*> temperatures;
  TextSensor lastAction;
};