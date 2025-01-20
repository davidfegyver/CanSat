#include "network.h"

Network::Network(Logger &i_logger) : logger(i_logger) {}

void Network::init()
{
    logger.addEvent(F("Initializing WiFi"));
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAPConfig(IPAddress(WIFI_IP), IPAddress(WIFI_IP), IPAddress(WIFI_SUBNET));
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL, 0, WIFI_MAX_CLIENTS);

    logger.addEvent(PSTR("WiFi MAC: ") + WiFi.macAddress());
    WiFi.setHostname(DEVICE_HOSTNAME);

    logger.addEvent(F("WiFi initialized successfully"));

    logger.addEvent(F("Initializing Webserver"));

    setupRoutes();

    webserver.begin();

    logger.addEvent(PSTR("Webserver started, connect at: http://") + WiFi.softAPIP().toString() + PSTR(":") + String(WEB_SERVER_PORT));
}


void Network::connectCamera(Camera * i_camera)
{
    camera = i_camera;
}

void Network::connectSdCard(MicroSd *i_sdCard)
{
  sdCard = i_sdCard;

}




void Network::setupRoutes()
{
    webserver.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
              { handleRoot(request); });

    webserver.onNotFound([this](AsyncWebServerRequest *request)
                      { handleNotFound(request); });

    webserver.on("/actions/capturePhoto", HTTP_POST, [this](AsyncWebServerRequest *request)
              { 
                if (camera)
                {
                  camera->capturePhoto();
                  request->send(200, "text/plain", "Photo captured");
                }
                else
                {
                  request->send(500, "text/plain", "Camera not connected");
                }

               });

    webserver.on("latest.jpg", HTTP_GET, [this](AsyncWebServerRequest *request)
              { 
                if (camera)
                {
                    
                }
                else
                {
                  request->send(500, "text/plain", "Camera not connected");
                }
               });
}

void Network::handleRoot(AsyncWebServerRequest *request)
{
    request->send(200, "text/plain", "Hello, world");
}

void Network::handleNotFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}
