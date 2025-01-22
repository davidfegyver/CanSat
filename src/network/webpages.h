#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "cfg.h"

void setupStaticFiles(AsyncWebServer& server);
void handleCacheRequest(AsyncWebServerRequest* request, const char* contentType, const char* data);

extern const char index_html[];
extern const char script_js[];
extern const char styles_css[];