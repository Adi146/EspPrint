#include "file_analyzer.h"

using namespace storage::analyzer;

void Fileinfo::fillJsonObject(JsonObject& obj) {
  obj["path"] = path;
  obj["size"] = size;
  obj["lastWrite"] = lastWrite;
}

Fileanalyzer::Fileanalyzer(std::string eventPrefix):
  Component(),
  CustomAPIDevice(),
  m_eventPrefix(eventPrefix){
}

Fileinfo Fileanalyzer::analyze(fs::File& file) {
  Fileinfo info;
  info.path = file.name();
  info.size = file.size();
  info.lastWrite = file.getLastWrite();

  return info;
}

void Fileanalyzer::setup() {
  register_service(&Fileanalyzer::fireListEvent, "list_files");
  register_service(&Fileanalyzer::deleteFile, "delete_file", {"file"});

  addFile("/");
}

void Fileanalyzer::fireListEvent() {
  DynamicJsonDocument doc(96 * m_files.size());

  for (auto i = 0; i < m_files.size(); i++) {
    JsonObject obj = doc.createNestedObject();
    m_files[i].fillJsonObject(obj);
  }

  std::string tmp;
  serializeJson(doc, tmp);

  fire_homeassistant_event("esphome." + m_eventPrefix + "_files", {
    { "files", tmp },
    { "totalSize", to_string(fsAdapter->getTotalSize()) },
    { "usedSize", to_string(fsAdapter->getUsedSize()) }
  });
}

void Fileanalyzer::addFile(fs::File& file) {
  if(file) {
    if (file.isDirectory()) {
      while(true) {
        File entry = file.openNextFile();
        if(!entry) {
          break;
        }
        addFile(entry);
        entry.close();
      }
    }
    else {
      ESP_LOGI("analzyer", "new file found: %s", file.name());
      m_files.push_back(analyze(file));
    }
  }
}

void Fileanalyzer::addFile(std::string path) {
  auto file = fs.open(path.c_str(), FILE_READ);
  if (file) {
    addFile(file);
  }
  file.close();
}

void Fileanalyzer::deleteFile(std::string path) {
  if(fs.remove(path.c_str())) {
    removeFromList(path);
  }

  fireListEvent();
}

void Fileanalyzer::removeFromList(std::string path) {
  for (auto it = m_files.begin(); it != m_files.end();) {
    if(it->path == path) {
      it = m_files.erase(it);
    }
    else {
      it++;
    }
  }
}