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

void Network::connectCamera(Camera *i_camera)
{
  camera = i_camera;
}

void Network::connectSdCard(MicroSd *i_sdCard)
{
  sdCard = i_sdCard;
}

void Network::connectBlinkingLed(BlinkingLed *i_led)
{
  led = i_led;
}

void Network::setupRoutes()
{
  setupStaticFiles(webserver);

  webserver.on("/action/camera/capture", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
    if (camera) {
      camera -> capturePhoto();
      request -> send(200, "text/plain", "Photo captured");
    } else {
      request -> send(500, "text/plain", "Camera not connected");
    } });

  webserver.on("/latest.jpg", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
    if (camera) {
      camera_fb_t * fb = camera -> getPhotoFrameBuffer();

      if (!fb) {
        request -> send(500, "text/plain", "No photo available");
        return;
      }

      AsyncWebServerResponse * response = request -> beginChunkedResponse("image/jpeg", [fb](uint8_t * buffer, size_t maxLen, size_t index) -> size_t {
        size_t chunkSize = maxLen;
        if (index + maxLen > fb -> len) {
          chunkSize = fb -> len - index;
        }
        memcpy(buffer, fb -> buf + index, chunkSize);
        return chunkSize;
      });

      response -> addHeader("Cache-Control", "no-cache");
      request -> send(response);
    } else {
      request -> send(500, "text/plain", "Camera not connected");
    } });

  webserver.on("/capture.jpg", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
    if (camera) {
      camera -> capturePhoto();
      camera_fb_t * fb = camera -> getPhotoFrameBuffer();

      if (!fb) {
        request -> send(500, "text/plain", "No photo available");
        return;
      }

      AsyncWebServerResponse * response = request -> beginChunkedResponse("image/jpeg", [fb](uint8_t * buffer, size_t maxLen, size_t index) -> size_t {
        size_t chunkSize = maxLen;
        if (index + maxLen > fb -> len) {
          chunkSize = fb -> len - index;
        }
        memcpy(buffer, fb -> buf + index, chunkSize);
        return chunkSize;
      });

      response -> addHeader("Cache-Control", "no-cache");
      request -> send(response);
    } else {
      request -> send(500, "text/plain", "Camera not connected");
    } });

  webserver.on("/action/getcrypto", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
    String response = "Crypto test\n";
    Crypto crypto;

                camera_fb_t * fb = camera -> getPhotoFrameBuffer();

    if (!fb) {
      request -> send(500, "text/plain", "No photo available");
      return;
    }

    std::vector<uint8_t> data(fb -> buf, fb -> buf + fb -> len);

    int n = crypto.calculate_n(data.size(), 1, 32);

    response += "n: " + String(n) + "\n";

    std::vector<std::vector<uint8_t>> split_data = crypto.split_vector(data, n);

    for (int i = 0; i < n; i++) {
      std::vector<uint8_t> hash = crypto.sha256(split_data[i]);

      response += "Hash " + String(i) + ": ";
      for (int j = 0; j < hash.size(); j++) {
        response += String(hash[j], HEX) + " ";
      }
      response += "\n";
    }
    
    request->send(200, "text/plain", response); });

  webserver.on("/action/led/toggle", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
      if (led) {
        led -> toggle();
        request -> send(200, "text/plain", "Led toggled");
        return;
      } 
        request -> send(500, "text/plain", "Led not connected"); });

  webserver.on("/action/led/blink/suspend", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
        if (led) {
          led -> suspendTask();
          request -> send(200, "text/plain", "Led blinking suspended");
          return;
        } 
          request -> send(500, "text/plain", "Led not connected"); });

  webserver.on("/action/led/blink/resume", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
          if (led) {
            led -> resumeTask();
            request -> send(200, "text/plain", "Led blinking resumed");
            return;
          } 
            request -> send(500, "text/plain", "Led not connected"); });

  webserver.onNotFound([this](AsyncWebServerRequest *request)
                       { handleNotFound(request); });
}

void Network::handleNotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}