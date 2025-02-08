#include "network.h"
#include "globals.h"

Network::Network() {}

void Network::init()
{
  SystemLog.addEvent(F("Initializing WiFi"));
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAPConfig(IPAddress(WIFI_IP), IPAddress(WIFI_IP), IPAddress(WIFI_SUBNET));
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL, 0, WIFI_MAX_CLIENTS);

  SystemLog.addEvent(PSTR("WiFi MAC: ") + WiFi.macAddress());
  WiFi.setHostname(DEVICE_HOSTNAME);

  SystemLog.addEvent(F("WiFi initialized successfully"));

  SystemLog.addEvent(F("Initializing Webserver"));

  setupRoutes();

  webserver.begin();

  SystemLog.addEvent(PSTR("Webserver started, connect at: http://") + WiFi.softAPIP().toString() + PSTR(":") + String(WEB_SERVER_PORT));
}

void Network::setupRoutes()
{
  setupStaticFiles(webserver);

  webserver.on("/action/SystemCamera/capture", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
      SystemCamera. capturePhoto();
      request -> send(200, "text/plain", "Photo captured"); });

  webserver.on("/latest.jpg", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
      camera_fb_t * fb = SystemCamera. getPhotoFrameBuffer();

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
      request -> send(response); });

  webserver.on("/capture.jpg", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
      SystemCamera. capturePhoto();
      camera_fb_t * fb = SystemCamera. getPhotoFrameBuffer();

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
      request -> send(response); });

  webserver.on("/action/getcrypto", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
    String response = "Crypto test\n";
    Crypto crypto;

                camera_fb_t * fb = SystemCamera. getPhotoFrameBuffer();

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
        flash_led .toggle();
        request -> send(200, "text/plain", "Led toggled");
        return; });

  webserver.on("/action/led/blink/suspend", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
          flash_led.suspendTask();
          request -> send(200, "text/plain", "Led blinking suspended");
          return; });

  webserver.on("/action/led/blink/resume", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
            flash_led. resumeTask();
            request -> send(200, "text/plain", "Led blinking resumed");
            return; });

  webserver.onNotFound([this](AsyncWebServerRequest *request)
                       { handleNotFound(request); });

  webserver.on("/System.log", HTTP_GET, [this](AsyncWebServerRequest *request)
               { 
                if( sd_card.getCardHealthy()){
                sd_card.sendFileToClient(request, SystemLog.getFilePath() + "/" + SystemLog.getFileName());

                }
                else{
                  request -> send(200, "text/plain", SystemLog.getFullLogMsg());
                } });

  webserver.on("/action/sd/format", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
    sd_card.formatCard();

    request->send(200, "text/plain", "SD card formatted, rebooting...");
    ESP.restart(); });

  webserver.on("/action/system/reboot", HTTP_GET, [this](AsyncWebServerRequest *request)
               {  

    request -> send(200, "text/plain", "Rebooting...");
    ESP.restart(); });

  webserver.on("/action/sd/getinfo", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
      String response = "SD card info\n";
      response += "Card size: " + String(sd_card. getCardSizeMB()) + " MB\n";
      response += "Total space: " + String(sd_card. getCardTotalMB()) + " MB\n";
      response += "Used space: " + String(sd_card. getCardUsedMB()) + " MB\n";
      response += "Free space: " + String(sd_card. getCardFreeMB()) + " MB\n";
      response += "Used space percent: " + String(sd_card. getUsedSpacePercent()) + "%\n";
      response += "Free space percent: " + String(sd_card. getFreeSpacePercent()) + "%\n";

      request -> send(200, "text/plain", response); });
}

void Network::handleNotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}
