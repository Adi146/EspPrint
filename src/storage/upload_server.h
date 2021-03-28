#pragma once

#include "esphome.h"
#include "FS.h"
#include "sd_gcode_sender.h"

using namespace esphome;

namespace storage {
  class UploadServer: public Component, public AsyncWebHandler {
  protected:
    web_server_base::WebServerBase* m_base;
    SDGCodeSender* m_sender;

  public:
    UploadServer(web_server_base::WebServerBase* base, SDGCodeSender* sender);

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
