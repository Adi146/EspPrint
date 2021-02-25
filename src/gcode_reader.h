#pragma once

#include "esphome.h"
#include <regex>
#include <string>
#include "gcode_sender.h"

using namespace esphome::custom_component;

#define get_reader(constructor) static_cast<GCodeReader *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

class GCodeReader : public Component, public UARTDevice {
private:
  std::regex okRgx = std::regex(R"(^ok (N(\d+) )?(P(\d+)) (B(\d+))$)");
  std::regex resendRgx = std::regex(R"(Resend: (\d+))");

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

protected:
  virtual bool processLine(std::string& line) {
    if (handleOK(line))
      return true;
      
    if (handleResend(line))
      return true;

    return false;
  }

  bool handleOK(std::string& line) {
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

  virtual bool handleResend(std::string& line) {
    std::smatch match;
    if (std::regex_search(line, match, resendRgx)) {
      sender->handleResend(atoi(match[1].str().c_str()));
      return true;
    }

    return false;
  }

public:
  GCodeReader(UARTComponent* parent, GCodeSender* sender) : 
  UARTDevice(parent),
  sender(sender) {
  }

  void loop() override {
    std::string line = readLine();
    if(line.length() > 0) {
      ESP_LOGI("gcode_reader", "RECV: %s", line.c_str());

      processLine(line);
    }
  }
};