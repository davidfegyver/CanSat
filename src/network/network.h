#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "cfg.h"
#include "logging/logger.h"
#include "micro_sd/micro_sd.h"
#include "camera/camera.h"

#include "crypto/crypto.h"
#include "led/blinkingLed.h"

#include <vector>

#include "webpages.h"

#include <ESPAsyncWebServer.h>
#include "lora/lora.h"

class Network
{
private:
  void setupRoutes();

  void handleNotFound(AsyncWebServerRequest *request);

public:
  Network();

  AsyncWebServer webserver{WEB_SERVER_PORT};

  void init();
};
