#pragma once

#include "esphome/core/component.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/GCodeQueue/GCodeQueue.h"
#include "esphome/components/GCodeSender/GCodeSender.h"
#include "threading.h"
#include <regex>
#include <string>

using namespace core::analyzer;
using namespace esphome;
using namespace esphome::uart;

namespace core {
  namespace communication {
    class GCodeReader : public Component, public UARTDevice, public util::Threading {
    private:
      std::regex m_okRgx = std::regex(R"(ok(( N(\d+))? (P(\d+)) (B(\d+)))?)");
      std::regex m_resendRgx = std::regex(R"(Resend: (\d+))");
      std::string m_busyStr = "echo:busy:";

      GCodeSender* m_sender;

      char m_readBuffer[MAX_GCODE_LENGTH];
      int m_readBuffer_ptr = 0;
      
      GCodeQueue* m_analyzerQueue;

    protected:
      bool handleOK(std::string& line);

      bool handleResend(std::string& line);

      bool handleBusy(std::string& line);

    public:
      GCodeReader(GCodeSender* sender, GCodeQueue* analyzerQueue);

      void setup() override;

      void threadLoop() override;

      bool readLine(std::string& line);
    };
  }
}