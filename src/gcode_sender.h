
#pragma once

#include "esphome.h"
#include <string>
#include "ring_buffer.h"
#include <chrono>

#define BUFFER_SIZE 50
#define RESEND_BUFFER_SIZE 50
#define MIN_DELAY_MS 10
#define MAX_DELAY_MS 20
#define RESERVED_PRINTER_GCODE_BUFFER 5

using namespace esphome::custom_component;
using namespace std::chrono;

#define get_sender(constructor) static_cast<GCodeSender *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

class GCodeSender : public Component, public UARTDevice {
protected:
  RingBuffer<std::string> buffer;
  RingBuffer<std::string> resendBuffer;

  int m_resendCounter = 0;
  bool resend = false;
  int throttleDelayMS = MIN_DELAY_MS;

  int curCommandBufferSize = 1;
  int curPlannerBufferSize = 1; 
  int maxCommandBufferSize = 1;
  int maxPlannerBufferSize = 1;

  high_resolution_clock::time_point lastCommandTimestamp = high_resolution_clock::now();

  std::string addLineNumberAndChecksum(std::string gcode) {
    uint64_t lineNumber = resendBuffer.getWritePtr() + 1; 
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
  resendBuffer(RESEND_BUFFER_SIZE) {
  }

  void setup() override {}

  void loop() override {
    high_resolution_clock::time_point now = high_resolution_clock::now();
    auto ms = duration_cast<milliseconds>(now - lastCommandTimestamp).count();

    if (curCommandBufferSize > RESERVED_PRINTER_GCODE_BUFFER && ms > throttleDelayMS) {
      if (resend) {
        sendGCodeForce(resendBuffer.pop());
        resend = !resendBuffer.empty();
      }
      else {
        if (!buffer.empty())
          sendGCode(buffer.pop());
      }

      lastCommandTimestamp = now;
      int linesWithoutResend = resendBuffer.getWritePtr() - resendBuffer.getReadPtr();
      if(linesWithoutResend > 0 && linesWithoutResend % 1000 == 0 && throttleDelayMS > MIN_DELAY_MS){
        throttleDelayMS--;
        ESP_LOGI("gcode_sender", "communication seems to be stable, trying to decrease throttle to %d ms", throttleDelayMS);
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
    resendBuffer.push(gcode);
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
    resendBuffer.setReadPtr(lineNumber);
    resend = true;
    m_resendCounter++;
    ESP_LOGW("gcode_sender", "resend requested for line %llu", lineNumber);
    if (throttleDelayMS < MAX_DELAY_MS) {
      throttleDelayMS++;
      ESP_LOGI("gcode_sender", "increase throttle to %d ms", throttleDelayMS);
    }
  }

  int getResendCounter() {
    return m_resendCounter;
  }
};