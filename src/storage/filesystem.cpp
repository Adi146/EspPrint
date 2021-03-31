#include "filesystem.h"
#include "ArduinoJson.h"

using namespace storage;

Filesystem::Filesystem(FileReader* fileReader, std::string eventPrefix):
  Component(),
  CustomAPIDevice(),
  m_fs(fileReader->getFS()),
  m_eventPrefix(eventPrefix){
}

std::vector<Fileinfo> Filesystem::listDirectory(File file) {
  std::vector<Fileinfo> files;

  while(file) {
    File entry =  file.openNextFile();
    if (!entry) {
      break;
    }

    if(entry.isDirectory()) {
      std::vector<Fileinfo> dirEntries = listDirectory(entry);
      files.insert(files.end(), dirEntries.begin(), dirEntries.end());
    }
    else {
      files.push_back({
        entry.name(),
        entry.size()
      });
    }
  }

  return files;
}

void Filesystem::setup() {
  register_service(&Filesystem::fireListEvent, "list_files");
  register_service(&Filesystem::deleteFile, "delete_file", {"file"});

  auto root = m_fs.open("/", FILE_READ);
  if (root) {
    m_files = listDirectory(root);
  }
}

void Filesystem::fireListEvent() {
  DynamicJsonDocument doc(1024);

  for (auto i = 0; i < m_files.size(); i++) {
    doc[i]["path"] = m_files[i].path;
    doc[i]["size"] = m_files[i].size;
  }

  std::string tmp;
  serializeJson(doc, tmp);

  fire_homeassistant_event("esphome." + m_eventPrefix + "_files", {
    {"files", tmp}
  });
}

void Filesystem::deleteFile(std::string file) {
  if(m_fs.remove(file.c_str())) {
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