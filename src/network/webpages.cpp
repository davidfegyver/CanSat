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
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CanSat debug controller</title>

    <link href="./styles.css" rel="stylesheet">
</head>

<body>
    <h1>CanSat debug controller</h1>


    <div class="section">
        <h2>Camera Controls</h2>
        <div class="buttons">
            <button class="button" onclick="control('camera/capture')">Capture Image</button>
            <button class="button" onclick="showImage('/latest.jpg')">Get Latest Image</button>
            <button class="button" onclick="showImage('/capture.jpg')">Capture & Show Image</button>
        </div>
    </div>

    <div class="section">
        <h2>Flash LED Controls</h2>
        <div class="buttons">
            <button class="button" onclick="control('led/toggle')">Toggle LED</button>
            <button class="button" onclick="control('led/blink/suspend')">Suspend LED Blink</button>
            <button class="button" onclick="control('led/blink/resume')">Resume LED Blink</button>
        </div>
    </div>

    <div class="section">
        <h2>Other Functions</h2>
        <div class="buttons">
            <button class="button" onclick="control('getcrypto')">Get Crypto Data</button>
        </div>
    </div>

    <div class="section">
        <h2>Output</h2>
        <div id="output"></div>
    </div>

    <script src="./script.js"></script>
</body>

</html>
)rawliteral";

const char script_js[] PROGMEM = R"rawliteral(
const output = document.getElementById('output');

async function control(action) {
    output.textContent = 'Sending request...';

    const response = await fetch(`/action/${action}`);

    const data = await response.text();
    output.innerHTML = data.replaceAll('\n', '<br>');
}

async function showImage(path) {
    output.textContent = 'Loading image...';
    const response = await fetch(path);
    
    if (!response.headers.get('content-type').startsWith('image')) {
        const data = await response.text();
        output.textContent = data;
        return;
    }

    const data = await response.blob();
    const url = URL.createObjectURL(data);
    output.innerHTML = `<img src="${url}" width="200">`;
}
)rawliteral";

const char styles_css[] PROGMEM = R"rawliteral(
body {
    font-family: "Roboto", sans-serif;

    background-color: #102435;
    color: #102435;

    margin: 0;
    padding: 20px;
}

h1 {
    text-align: center;
    color: #e0e0df;
}

.button {
    display: block;
    width: 200px;
    margin: 10px auto;
    padding: 10px;
    text-align: center;
    background-color: #c09e6c;
    color: #e0e0df;
    border: none;
    border-radius: 5px;
    cursor: pointer;
}

.output {
    text-align: center;
    margin-top: 20px;
    font-size: 1.2em;
}

.section {
    margin: 20px 0;
    padding: 20px;
    border-radius: 8px;
    background-color: #e6e6e6;
}

.section h2 {
    margin-top: 0;
}

.buttons {
    display: flex;
    flex-wrap: wrap;
    gap: 10px;
    justify-content: center;
}

.hidden {
    display: none;
}
)rawliteral";

void setupStaticFiles(AsyncWebServer& server) {
server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { handleCacheRequest(request, "text/html", index_html); });
server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest* request) { handleCacheRequest(request, "application/javascript", script_js); });
server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest* request) { handleCacheRequest(request, "text/css", styles_css); });
}