#include "upload_server.h"

using namespace storage;

UploadServer::UploadServer(web_server_base::WebServerBase* base, FileReader* fileReader, analyzer::Fileanalyzer* fileanalyzer):  
  m_base(base), 
  m_fileReader(fileReader),
  m_fileanalyzer(fileanalyzer) {
}

void UploadServer::setup() {
  this->m_base->init();
  this->m_base->add_handler(this);
}
    
bool UploadServer::canHandle(AsyncWebServerRequest *request) {
  if (!request->url().startsWith("/api/"))
    return false;

  return true ;
}

void UploadServer::handleRequest(AsyncWebServerRequest *request) {
  if (request->url().startsWith("/api/version") && request->method() == HTTP_GET) { // Fake Ocoprint API for slicer upload
    request->send(200, "application/json", R"({"api": "0.1", "server": "0.1", "text": "OctoPrint 0.1"})");
    return;
  }
  if (request->url().startsWith("/api/files/local") && request->method() == HTTP_POST) {
    auto printParam = request->getParam("print", true, false);
    auto fileParam = request->getParam("file", true, true);
    if (fileParam && printParam && printParam->value() == "true") {
      m_fileReader->print(("/" + fileParam->value()).c_str());
    }
  }
}

void UploadServer::handleUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
  std::string path("/");
  path += filename.c_str();

  if (!index) {
    ESP_LOGI("upload", "Upload Request %s", path.c_str());
    request->_tempFile = fs.open(path.c_str(), FILE_WRITE);
  }

  if (!request->_tempFile) {
    ESP_LOGE("sdcard", "Cannot write File");
    request->send(500, "text/plain", "Cannot write File");
    return;
  }

  request->_tempFile.write(data, len);
  if (final) {
    ESP_LOGI("upload", "Upload complete %s", path.c_str());
    request->_tempFile.close();

    m_fileanalyzer->removeFromList(path);
    m_fileanalyzer->addFile(path);
    m_fileanalyzer->fireListEvent();

    request->send(201, "text/plain", "Upload Success");
  }
}