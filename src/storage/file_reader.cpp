#include "file_reader.h"

using namespace storage;

FileReader::FileReader(GCodeSender* sender, Filesystem* filesystem) :
  Component(),
  m_sender(sender),
  m_fs(filesystem->getFS()) {
}

std::string FileReader::readNextGCode() {
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

void FileReader::loop() {
  m_sender->m_bufferMutex.lock();
  if (m_file.available() && !m_sender->m_buffer.full()) {
    std::string gcode = readNextGCode();
    if (!gcode.empty()) {
      m_sender->m_buffer.push(gcode);
    }
  }
  m_sender->m_bufferMutex.unlock();
}

void FileReader::print(std::string filename) {
  if (m_file.available()) {
    ESP_LOGW("file", "Already printing, ignored");
    return;
  }

  ESP_LOGI("file", "Start printing from SD-Card %s", filename.c_str());

  m_file = m_fs.open(filename.c_str());
  if (!m_file) {
    ESP_LOGE("file", "File does not exist: %s", filename.c_str());
    return;
  }

  m_sender->reset();
}

void FileReader::stop() {
  m_sender->m_bufferMutex.lock();
  m_file.close();
  m_sender->m_buffer.setReadPtr(m_sender->m_buffer.getWritePtr());

  m_sender->m_buffer.push("M104 S0");
  m_sender->m_buffer.push("M140 S0");
  m_sender->m_buffer.push("G28 X Y");
  m_sender->m_buffer.push("M84");
  m_sender->m_bufferMutex.unlock();
}