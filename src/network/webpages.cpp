#include "webpages.h"


void handleCacheRequest(AsyncWebServerRequest* request, const char* contentType, const char* data) {
  AsyncWebServerResponse* response = request->beginChunkedResponse(contentType, [data](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
    const char* dataStart = data + index;
    size_t dataLeft = strlen(data) - index;
    size_t chunkSize = dataLeft < maxLen ? dataLeft : maxLen; 
    memcpy(buffer, dataStart, chunkSize); 
    return chunkSize;
  });
  
  response->addHeader("Cache-Control", "public, max-age=" + String(WEB_CACHE_INTERVAL));
  request->send(response);
}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <title>Hello</title>
</head>

<body>
    <h1>Hello, World!</h1>
</body>
)rawliteral";

const char index_js[] PROGMEM = R"rawliteral(

)rawliteral";

const char styles_css[] PROGMEM = R"rawliteral(

)rawliteral";

void setupStaticFiles(AsyncWebServer& server) {
server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { handleCacheRequest(request, "text/html", index_html); });
server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest* request) { handleCacheRequest(request, "application/javascript", index_js); });
server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest* request) { handleCacheRequest(request, "text/css", styles_css); });
}