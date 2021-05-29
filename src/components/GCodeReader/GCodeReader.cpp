#include "GCodeReader.h"

using namespace core::communication;

GCodeReader::GCodeReader(GCodeSender* sender, GCodeQueue* analyzerQueue): 
  UARTDevice(), 
  m_sender(sender),
  m_analyzerQueue(analyzerQueue) {
}

void GCodeReader::setup() {
  Threading::setup(8 * 1024, configMAX_PRIORITIES - 1, 1);
}

bool GCodeReader::readLine(std::string& line) {
  for (int i = available(); i > 0; i--) {
    m_readBuffer[m_readBuffer_ptr] = read();
    if (m_readBuffer[m_readBuffer_ptr] == '\n') {
      m_readBuffer[m_readBuffer_ptr] = 0;
      m_readBuffer_ptr = 0;
      line = std::string(m_readBuffer);
      return true;
    } 
    else if(m_readBuffer[m_readBuffer_ptr] < 128) {
      m_readBuffer_ptr++;
    }
  }

  return false;
}

bool GCodeReader::handleOK(std::string& line) {
  std::smatch match;
  if (std::regex_search(line, match, m_okRgx)) {
    uint64_t lineNumber = match[3].matched ? atoi(match[3].str().c_str()) : 0;
    int plannerBuffer = match[5].matched ? atoi(match[5].str().c_str()) : -1;
    int commandBuffer = match[7].matched ? atoi(match[7].str().c_str()) : -1;

    m_sender->handleOK(plannerBuffer, commandBuffer, lineNumber);
    return true;
  }
    
  return false;
}

bool GCodeReader::handleResend(std::string& line) {
  std::smatch match;
  if (std::regex_search(line, match, m_resendRgx)) {
    m_sender->handleResend(atoi(match[1].str().c_str()));
    return true;
  }

  return false;
}

bool GCodeReader::handleBusy(std::string& line) {
  if (line.rfind(m_busyStr, 0) == 0) {
    m_sender->handleBusy();
    return true;
  }

  return false;
}

void GCodeReader::threadLoop() {
  std::string line;

  if (readLine(line)) {
    handleOK(line) || handleBusy(line) || handleResend(line);

    m_analyzerQueue->handleLine(line, GCodeSource::READER);
  }
}