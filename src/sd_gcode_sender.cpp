#include "sd_gcode_sender.h"

SDGCodeSender::SDGCodeSender(UARTComponent *parent, fs::FS &fs) : GCodeSender(parent), m_fs(fs) {}

std::string SDGCodeSender::readNextGCode() {
  char buffer[128] = "";
  for (int i = 0; i < sizeof(buffer) && m_file.available(); i++) {
    char c = m_file.read();
    switch (c) {
    case ';':
      m_file.readStringUntil('\n');
    case '\n':
      if (i > 1)
        return std::string(buffer);
      i = -1;
    default:
      buffer[i] = c;
      if (i < sizeof(buffer))
        buffer[i + 1] = 0;
    }
  }

  return buffer;
}

void SDGCodeSender::loop() {
  if (m_file.available() && !m_buffer.full()) {
    std::string gcode = readNextGCode();
    if (!gcode.empty()) {
      bufferGCode(gcode);

      for (auto sensor: m_sensors) {
        if (sensor->handleLine(gcode))
          break;
      }
    }
  }

  GCodeSender::loop();
}

void SDGCodeSender::print(std::string filename) {
  ESP_LOGI("sdcard", "Start printing from SD-Card %s", filename.c_str());

  m_file = m_fs.open(filename.c_str());
  if (!m_file) {
    ESP_LOGE("sdcard", "File does not exist: %s", filename.c_str());
    return;
  }

  m_buffer.reset();
  m_resendBuffer.reset();
  sendGCodeForce("M110 N0");
}

void SDGCodeSender::stop() {
  m_file.close();

  sendGCode("M104 S0");
  sendGCode("M140 S0");
  sendGCode("G28 X Y");
  sendGCode("M84");
}