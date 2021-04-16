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

  auto root = fs.open("/", FILE_READ);
  if (root) {
    addFile(root);
  }
  root.close();
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

void Fileanalyzer::deleteFile(std::string file) {
  if(fs.remove(file.c_str())) {
    for (auto it = m_files.begin(); it != m_files.end();) {
      if(it->path == file) {
        it = m_files.erase(it);
      }
      else {
        it++;
      }
    }
  }

  fireListEvent();
}