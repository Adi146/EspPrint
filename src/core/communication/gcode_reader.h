#pragma once

#include "esphome.h"
#include "analyzer/gcode_queue.h"
#include "gcode_sender.h"
#include "ring_buffer.h"
#include "threading.h"
#include <regex>
#include <string>

using namespace core::analyzer;
using namespace esphome;
using namespace esphome::uart;
using namespace esphome::custom_component;

#define get_reader(constructor) static_cast<GCodeReader *>(const_cast<CustomComponentConstructor *>(&constructor)->get_component(0))

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
      GCodeReader(UARTComponent* parent, GCodeSender* sender, GCodeQueue* analyzerQueue);

      void setup() override;

      void threadLoop() override;

      bool readLine(std::string& line);
    };
  }
}