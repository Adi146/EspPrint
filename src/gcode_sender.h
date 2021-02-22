
#pragma once

#include "esphome.h"
#include <string>
#include "ring_buffer.h"

#define BUFFER_SIZE 50

using namespace esphome::custom_component;

#define get_sender(constructor) static_cast<GCodeSender *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

class GCodeSender : public Component, public UARTDevice {
protected:
  RingBuffer<std::string> buffer;
  RingBuffer<std::string> transmittedBuffer;

  bool resend = false;

  int curCommandBufferSize = 1;
  int curPlannerBufferSize = 1; 
  int maxCommandBufferSize = 1;
  int maxPlannerBufferSize = 1;

  std::string addLineNumberAndChecksum(std::string gcode) {
    uint64_t lineNumber = transmittedBuffer.getWritePtr() + 1; 
    gcode = "N" + to_string(lineNumber) + " " + gcode;

    int checksum = 0;
    for (auto it = gcode.cbegin(); it != gcode.cend(); it++)
      checksum ^= *it;

    return gcode + "*" + to_string(checksum);
  }

public:
  GCodeSender(UARTComponent *parent) : 
  UARTDevice(parent),
  buffer(BUFFER_SIZE),
  transmittedBuffer(BUFFER_SIZE) {
  }

  void setup() override {}

  void loop() override {
    if (curCommandBufferSize > 5) {
      if (resend) {
        sendGCodeForce(transmittedBuffer.pop());
        resend = !transmittedBuffer.empty();
      }
      else {
        if (!buffer.empty())
          sendGCode(buffer.pop());
      }
    }
  }

  void sendGCodeForce(std::string gcode) {
    ESP_LOGI("gcode_sender", "SEND: %s", gcode.c_str());
    write_str((gcode + "\n").c_str());
    curCommandBufferSize--;
  }

  void sendGCode(std::string gcode) {
    gcode = addLineNumberAndChecksum(gcode);
    transmittedBuffer.push(gcode);
    sendGCodeForce(gcode);
  }

  bool bufferGCode(std::string& gcode) {
    if (!buffer.full()) {
      buffer.push(gcode);
      return true;
    }

    return false;
  }

  void handleOK(int plannerBuffer, int commandBuffer, uint64_t lineNumber) {
    curPlannerBufferSize = plannerBuffer;
    curCommandBufferSize = commandBuffer;

    if (plannerBuffer > maxPlannerBufferSize)
      maxPlannerBufferSize = plannerBuffer;
    if (commandBuffer > maxCommandBufferSize)
      maxCommandBufferSize = commandBuffer;
  }

  void handleResend(uint64_t lineNumber) {
    transmittedBuffer.setReadPtr(lineNumber);
    resend = true;
  }
};