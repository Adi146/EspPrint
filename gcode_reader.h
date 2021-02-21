#pragma once

#include "esphome.h"
#include <regex>
#include <string>
#include <vector>
#include "gcode_sender.h"

using namespace esphome::custom_component;

#define get_reader(constructor) static_cast<GCodeReader *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

class GCodeReader : public Component, public UARTDevice {
private:
  std::regex okRgx = std::regex(R"(^ok (N(\d+) )?(P(\d+)) (B(\d+))$)");
  std::regex resendRgx = std::regex(R"(Resend: (\d+))");
  std::regex temperatureRgx = std::regex(R"(([BTA](\d*)):\s*([-+]?[0-9]*\.?[0-9]+)(\s*\/?\s*([-+]?[0-9]*\.?[0-9]+))?)");
  std::regex sdcardPrintingRgx = std::regex(R"((Not SD printing|SD printing byte)( (\d*)\/(\d*))?)");
  std::string actionStr = "//action:notification ";

  GCodeSender* sender;

  char readBuffer[256];
  int readBuffer_ptr = 0;

  std::string readLine() {
    for (int i = available(); i > 0; i--) {
      readBuffer[readBuffer_ptr] = read();
      if (readBuffer[readBuffer_ptr] == '\n' || readBuffer[readBuffer_ptr] >= 128) {
        readBuffer[readBuffer_ptr] = 0;
        readBuffer_ptr = 0;
        return std::string(readBuffer);
      }

      readBuffer_ptr++;
    }

    return std::string();
  }
public:
  std::vector<Sensor*> temperatures;
  BinarySensor sdCardPrinting;
  TextSensor lastAction;

  GCodeReader(UARTComponent* parent, GCodeSender* sender, int numTemperatures) : UARTDevice(parent) {
    this->sender = sender;

    temperatures.reserve(numTemperatures * 2);
    for(int i = 0; i < numTemperatures * 2; i++) {
      temperatures.push_back(new Sensor());
    }
  }

  void setup() override {}

  void loop() override {
    std::string line = readLine();
    if(line.length() > 0) {
      ESP_LOGI("gcode_reader", "RECV: %s", line.c_str());

      if (handleOK(line))
        return;
      
      if (handleResend(line))
        return;

      
        //if (updatePrintProgress(line))
        //  return;

      if (updateTemperatureSensors(line))
        return;

      if (handleAction(line))
        return;
    }
  }

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

  bool updatePrintProgress(std::string line) {
    std::smatch match;

    if (std::regex_search(line, match, sdcardPrintingRgx)) {
      sdCardPrinting.publish_state(match[0].str() == "SD printing byte");
      return true;
    }

    return false;
  }

  bool handleOK(std::string line) {
    std::smatch match;

    if (std::regex_search(line, match, okRgx)) {
      int lineNumber = match[2].matched ? atoi(match[2].str().c_str()) : -1;
      int plannerBuffer = atoi(match[4].str().c_str());
      int commandBuffer = atoi(match[6].str().c_str());

      sender->handleOK(plannerBuffer, commandBuffer, lineNumber);
      return true;
    }

    return false;
  }

  bool handleResend(std::string line) {
    std::smatch match;
    if (std::regex_search(line, match, resendRgx)) {
      sender->handleResend(atoi(match[1].str().c_str()));
      return true;
    }

    return false;
  }

  bool handleAction(std::string line) {
    if (line.rfind(actionStr, 0) == 0) {
      lastAction.publish_state(line.substr(actionStr.length()));
      return true;
    }

    return false;
  }
};