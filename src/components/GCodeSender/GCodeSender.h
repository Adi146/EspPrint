
#pragma once

#include "esphome/core/component.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/GCodeQueue/GCodeQueue.h"
#include "threading.h"
#include <string>
#include <deque>

#define INPUT_BUFFER_SIZE 50
#define MAX_GCODE_LENGTH 256
#define EMERGENCY_GCODE { "M108", "M112", "M410", "M876" }

using namespace core::analyzer;
using namespace esphome;
using namespace esphome::api;
using namespace esphome::uart;

namespace core {
  namespace communication {
    class GCodeSender : public Component, public CustomAPIDevice, public UARTDevice, public util::Threading {

    private:
      int m_printerGCodeBufferSize = 1;

      uint64_t m_sentLineNumber = 0;
      uint64_t m_processedLineNumber = 0;

      std::queue<std::string> m_inputBuffer;
      std::deque<std::string> m_resendBuffer;

      int m_resendCounter = 0;
      int m_timeoutCounter = 0;
      int m_okPlannerBuffer = 0;
      int m_okGCodeBuffer = 0;

      uint32_t m_lastCommandTimestamp;

      std::mutex m_inputBufferMutex;
      
      GCodeQueue* m_analyzerQueue;

      void ok(int plannerBuffer, int commandBuffer, uint64_t lineNumber, bool timeout);

      void _sendGCode(std::string gcode, uint64_t lineNumber);

    public:
      GCodeSender(GCodeQueue* analyzerQueue);

      void setup() override;

      void threadLoop() override;

      bool sendGCode(std::string gcode, int timeout);

      void sendGCode(std::string gcode) {
        sendGCode(gcode, 500);
      }

      void reset();

      void handleOK(int plannerBuffer, int commandBuffer, uint64_t lineNumber);

      void handleResend(uint64_t lineNumber);

      void handleBusy();

      int getResendCounter() {
        return m_resendCounter;
      }

      int getTimeoutCounter() {
        return m_timeoutCounter;
      }

      int getFreePlannerBuffer() {
        return m_okPlannerBuffer;
      }

      int getFreeGCodeBuffer() {
        return m_okGCodeBuffer;
      }

      void setResendBufferSize(int val) {
        m_printerGCodeBufferSize = val;
      }
    };
  }
}
