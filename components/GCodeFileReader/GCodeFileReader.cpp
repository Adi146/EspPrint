#include "GCodeFileReader.h"

using namespace storage;

GCodeFileReader::GCodeFileReader(GCodeSender* sender, std::vector<std::string> cancelGCodes) :
  Component(),
  CustomAPIDevice(),
  m_sender(sender),
  m_cancelGCodes(cancelGCodes) {
}

void GCodeFileReader::readNextGCode(std::string& gcode) {
  char buffer[MAX_GCODE_LENGTH] = "";
  for (int i = 0; i < sizeof(buffer) && m_file.available(); i++) {
    char c = m_file.read();
    switch (c) {
    case ';':
      m_file.readStringUntil('\n');
    case '\n':
      if (i > 1)
        gcode = std::string(buffer);
        return;
      i = -1;
    default:
      buffer[i] = c;
      if (i < sizeof(buffer))
        buffer[i + 1] = 0;
    }
  }

  gcode = std::string(buffer);
}

void GCodeFileReader::setup() {
  register_service(&GCodeFileReader::print, "print_file", {"file"});
  register_service(&GCodeFileReader::stop, "cancle_print");

  Threading::setup(4 * 1024, 1, 1);
}

void GCodeFileReader::threadLoop() {
  while (m_file.available() && m_tmpGCode.empty()) {
    readNextGCode(m_tmpGCode);
  }

  if(!m_tmpGCode.empty() && m_sender->sendGCode(m_tmpGCode, 0)) {
    m_tmpGCode = "";
  }
}

void GCodeFileReader::print(std::string filename) {
  if (m_file.available()) {
    ESP_LOGW("file", "Already printing, ignored");
    return;
  }

  ESP_LOGI("file", "Start printing from SD-Card %s", filename.c_str());

  m_file = fs.open(filename.c_str());
  if (!m_file) {
    ESP_LOGE("file", "File does not exist: %s", filename.c_str());
    return;
  }

  m_sender->reset();
}

void GCodeFileReader::stop() {
  m_file.close();
  m_tmpGCode = "";

  for (auto it = m_cancelGCodes.begin(); it != m_cancelGCodes.end(); it++){
    m_sender->sendGCode(*it);
  }
}