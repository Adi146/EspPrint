#pragma once

#include "esphome.h"
#include "fs_adapter.h"
#include "file_reader.h"
#include "file_analyzer.h"

using namespace esphome;

namespace storage {
  class UploadServer: public Component, public AsyncWebHandler {
  protected:
    web_server_base::WebServerBase* m_base;
    FileReader* m_fileReader;
    analyzer::Fileanalyzer* m_fileanalyzer;

  public:
    UploadServer(web_server_base::WebServerBase* base, FileReader* fileReader, analyzer::Fileanalyzer* fileanalyzer);

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
