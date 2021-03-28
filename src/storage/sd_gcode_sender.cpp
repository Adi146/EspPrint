#include "sd_gcode_sender.h"

using namespace storage;

SDGCodeSender::SDGCodeSender(UARTComponent *parent, int resendBufferSize, fs::FS &fs) :
  GCodeSender(parent, resendBufferSize),
  m_fs(fs) {
}

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
  m_bufferMutex.lock();
  if (m_file.available() && !m_buffer.full()) {
    std::string gcode = readNextGCode();
    if (!gcode.empty()) {
      m_buffer.push(gcode);
    }
  }
  m_bufferMutex.unlock();

  GCodeSender::loop();
}

void SDGCodeSender::print(std::string filename) {
  if (m_file.available()) {
    ESP_LOGW("sdcard", "Already printing, ignored");
    return;
  }

  ESP_LOGI("sdcard", "Start printing from SD-Card %s", filename.c_str());

  m_file = m_fs.open(filename.c_str());
  if (!m_file) {
    ESP_LOGE("sdcard", "File does not exist: %s", filename.c_str());
    return;
  }

  reset();
}

void SDGCodeSender::stop() {
  m_bufferMutex.lock();
  m_file.close();
  m_buffer.setReadPtr(m_buffer.getWritePtr());

  m_buffer.push("M104 S0");
  m_buffer.push("M140 S0");
  m_buffer.push("G28 X Y");
  m_buffer.push("M84");
  m_bufferMutex.unlock();
}