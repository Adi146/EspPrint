#include "FileAnalyzer.h"

using namespace storage;

void Fileinfo::fillJsonObject(JsonObject& obj) {
  obj["path"] = path;
  obj["size"] = size;
  obj["lastWrite"] = lastWrite;
}

FileAnalyzer::FileAnalyzer():
  Component(),
  CustomAPIDevice(){
}

Fileinfo FileAnalyzer::analyze(fs::File& file) {
  Fileinfo info;
  info.path = file.name();
  info.size = file.size();
  info.lastWrite = file.getLastWrite();

  return info;
}

void FileAnalyzer::setup() {
  register_service(&FileAnalyzer::fireListEvent, "list_files");
  register_service(&FileAnalyzer::deleteFile, "delete_file", {"file"});

  addFile("/");
}

void FileAnalyzer::fireListEvent() {
  DynamicJsonDocument doc(160 + 96 * m_files.size());

  for (auto i = 0; i < m_files.size(); i++) {
    JsonObject obj = doc.createNestedObject();
    m_files[i].fillJsonObject(obj);
  }

  std::string tmp;
  serializeJson(doc, tmp);

  fire_homeassistant_event("esphome.espprint_files", {
    { "device_id", App.get_name() },
    { "files", tmp },
    { "totalSize", to_string(fsAdapter->getTotalSize()) },
    { "usedSize", to_string(fsAdapter->getUsedSize()) }
  });
}

void FileAnalyzer::addFile(fs::File& file) {
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

void FileAnalyzer::addFile(std::string path) {
  auto file = fs.open(path.c_str(), FILE_READ);
  if (file) {
    addFile(file);
  }
  file.close();
}

void FileAnalyzer::deleteFile(std::string path) {
  if(fs.remove(path.c_str())) {
    removeFromList(path);
  }

  fireListEvent();
}

void FileAnalyzer::removeFromList(std::string path) {
  for (auto it = m_files.begin(); it != m_files.end();) {
    if(it->path == path) {
      it = m_files.erase(it);
    }
    else {
      it++;
    }
  }
}