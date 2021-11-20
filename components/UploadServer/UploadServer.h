#pragma once

#include "esphome/core/component.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include "esphome/components/FileAnalyzer/FileAnalyzer.h"
#include "esphome/components/GCodeFileReader/GCodeFileReader.h"

using namespace esphome;

namespace storage {
  class UploadServer: public Component, public AsyncWebHandler {
  protected:
    web_server_base::WebServerBase* m_base;
    GCodeFileReader* m_fileReader;
    FileAnalyzer* m_fileanalyzer;

  public:
    UploadServer(web_server_base::WebServerBase* base, GCodeFileReader* fileReader, FileAnalyzer* fileanalyzer);

    void setup() override;

    float get_setup_priority() const override {
      return setup_priority::WIFI - 1.0f;
    }

    bool canHandle(AsyncWebServerRequest *request) override;

    bool isRequestHandlerTrivial() override { 
      return false; 
    }

    void handleRequest(AsyncWebServerRequest *request) override;

    void handleUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) override;
  };
}
