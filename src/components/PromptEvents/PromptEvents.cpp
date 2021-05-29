#include "PromptEvents.h"
#include "ArduinoJson.h"

using namespace action;

PromptEvents::PromptEvents() :
  GCodeAnalyzer(),
  Component(),
  CustomAPIDevice() {
}

void PromptEvents::handleLine(std::string& gcode, GCodeSource source) {
  if (source != GCodeSource::READER) {
    return;
  }

  if (gcode.rfind(m_promptBegin, 0) == 0) {
    m_message = gcode.substr(m_promptBegin.length() + 1);
    ESP_LOGI("action", "got message %s", m_message.c_str());
    return;
  }

  if (gcode.rfind(m_promptButton, 0) == 0) {
    std::string button = gcode.substr(m_promptButton.length() + 1);
    ESP_LOGI("action", "got button %s", button.c_str());
    m_buttons.push_back(button);
    return;
  }

  if (gcode.rfind(m_promptShow, 0) == 0) {
    fireEvent();
    return;
  }
}

void PromptEvents::fireEvent() {
  ESP_LOGI("action", "fire prompt event %s", m_message.c_str());

  DynamicJsonDocument doc(MAX_GCODE_LENGTH * m_buttons.size());

  for(int i = 0; i < m_buttons.size(); i++) {
    char buffer[MAX_GCODE_LENGTH];
    snprintf(buffer, sizeof(buffer), "M876 S%d", i);

    doc[i]["title"] = m_buttons[i];
    doc[i]["gcode"] = std::string(buffer);
  }

  std::string tmp;
  serializeJson(doc, tmp);

  fire_homeassistant_event("esphome.espprint_action", {
    { "device_id", App.get_name() },
    { "printer_name", m_printerName},
    { "message", m_message },
    { "buttons", tmp }
  });

  m_message = "";
  m_buttons = std::vector<std::string>();
}