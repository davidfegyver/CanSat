#include <Arduino.h>
#include <FS.h>
#include <WiFi.h>
#include "cfg.h"
#include "logging/logger.h"
#include "micro_sd/micro_sd.h"
#include "camera/camera.h"

#include "webpages.h"

#include <ESPAsyncWebServer.h>


class Network
{
private:
  Logger &logger;
  
  Camera *camera = nullptr;
  MicroSd *sdCard = nullptr;

  void setupRoutes();
  
  void handleNotFound(AsyncWebServerRequest *request);

public:
  Network(Logger &i_logger);


  AsyncWebServer webserver{WEB_SERVER_PORT};

  void init();

  void connectCamera(Camera *camera);
  void connectSdCard(MicroSd *sdCard);
};
