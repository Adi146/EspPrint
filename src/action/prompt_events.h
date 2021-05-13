#pragma once

#include "esphome.h"
#include "communication/gcode_sender.h"
#include "analyzer/gcode_analyzer.h"

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
    std::string m_printerName;

    void fireEvent();

  public:
    PromptEvents(std::string printerName);

    void handleLine(std::string& gcode, GCodeSource source) override;
  };
}