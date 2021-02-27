#include "upload_server.h"

using namespace util;

UploadServer::UploadServer(web_server_base::WebServerBase *base, fs::FS &fs):  m_base(base), m_fs(fs) {
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
}

void UploadServer::handleUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) {
    ESP_LOGI("upload", "Upload Request %s", filename.c_str());
    request->_tempFile = m_fs.open('/' + filename, FILE_WRITE);
  }

  if (!request->_tempFile) {
    ESP_LOGE("sdcard", "Cannot write File");
    request->send(500, "text/plain", "Cannot write File");
    return;
  }

  request->_tempFile.write(data, len);
  if (final) {
    ESP_LOGI("upload", "Upload complete %s", filename.c_str());
    request->_tempFile.close();
    request->send(201, "text/plain", "Upload Success");
  }
}