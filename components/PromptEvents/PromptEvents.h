#pragma once

#include "esphome/core/component.h"
#include "esphome/core/application.h"
#include "esphome/components/logger/logger.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/GCodeSender/GCodeSender.h"
#include "esphome/components/GCodeQueue/GCodeAnalyzer.h"

using namespace esphome;
using namespace esphome::api;
using namespace core::analyzer;

namespace action {
  class PromptEvents: public GCodeAnalyzer, public Component, public CustomAPIDevice {
  private:
    std::string m_promptBegin = "//action:prompt_begin";
    std::string m_promptButton = "//action:prompt_button";
    std::string m_promptShow = "//action:prompt_show";
    std::string m_promptEnd = "//action:prompt_end";

    std::vector<std::string> m_buttons;
    std::string m_message;
    std::string m_printerName = App.get_name();

    void fireEvent();

  public:
    PromptEvents();

    void handleLine(std::string& gcode, GCodeSource source) override;

    void setPrinterName(const std::string& name) {
      m_printerName = name;
    }
  };
}