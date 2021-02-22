#pragma once

#include "esphome.h"
#include "FS.h"
#include "SD_MMC.h"

class UploadServer: public Component, public AsyncWebHandler {
protected:
  web_server_base::WebServerBase *base_;
  fs::FS fs_;

public:
  UploadServer(web_server_base::WebServerBase *base, fs::FS &fs) : 
  base_(base), fs_(fs) {}

  void setup() override {
    this->base_->init();
    this->base_->add_handler(this);
  }

  void loop() override {}

  float get_setup_priority() const override {
    return setup_priority::WIFI - 1.0f;
  }

  bool canHandle(AsyncWebServerRequest *request) override {
    if (!request->url().startsWith("/api/")) {
      return false;
    }
    return true ;
  }

  bool isRequestHandlerTrivial() override { 
    return false; 
  }

  void handleRequest(AsyncWebServerRequest *request) override {
    if (request->url().startsWith("/api/version") && request->method() == HTTP_GET) { // Fake Ocoprint API for slicer upload
      request->send(200, "application/json", R"({"api": "0.1", "server": "0.1", "text": "OctoPrint 0.1"})");
      return;
    }
  }

  void handleUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      ESP_LOGI("upload", "Upload Request %s", filename.c_str());
      request->_tempFile = fs_.open('/' + filename, FILE_WRITE);
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
};