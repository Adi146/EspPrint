#include "filesystem.h"

using namespace storage;

void Fileinfo::fillJsonObject(JsonObject& obj) {
  obj["path"] = path;
  obj["size"] = size;
  obj["lastWrite"] = lastWrite;
}

Filesystem::Filesystem(fs::FS& fs, std::string eventPrefix):
  Component(),
  CustomAPIDevice(),
  m_fs(fs),
  m_eventPrefix(eventPrefix){
}

std::vector<Fileinfo> Filesystem::listDirectory(fs::File& directory) {
  std::vector<Fileinfo> files;

  while(directory) {
    File entry =  directory.openNextFile();
    if (!entry) {
      break;
    }

    if(entry.isDirectory()) {
      std::vector<Fileinfo> dirEntries = listDirectory(entry);
      files.insert(files.end(), dirEntries.begin(), dirEntries.end());
    }
    else {
      files.push_back(analyze(entry));
    }
  }

  return files;
}

Fileinfo Filesystem::analyze(fs::File& file) {
  Fileinfo info;
  info.path = file.name();
  info.size = file.size();
  info.lastWrite = file.getLastWrite();

  return info;
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
  DynamicJsonDocument doc(96 * m_files.size());

  for (auto i = 0; i < m_files.size(); i++) {
    JsonObject obj = doc.createNestedObject();
    m_files[i].fillJsonObject(obj);
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