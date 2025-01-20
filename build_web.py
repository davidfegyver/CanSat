import os

web_folder = "web"
output_file = "src/network/webpages.h"

header_lines = ["#pragma once", "#include <Arduino.h>", "#include <ESPAsyncWebServer.h>", "#include \"cfg.h\"",""]


header_lines.append("void setupStaticFiles(AsyncWebServer& server);")
header_lines.append("void handleCacheRequest(AsyncWebServerRequest* request, const char* contentType, const char* data);")
header_lines.append("")

code_lines = ["#include \"webpages.h\"", ""]


handleCacheRequest = """
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
"""

code_lines.append(handleCacheRequest)
code_lines.append("")

for filename in os.listdir(web_folder):
    file_path = os.path.join(web_folder, filename)

    header_lines.append(f"extern const char {filename.replace('.', '_')}[];")

for filename in os.listdir(web_folder):
    file_path = os.path.join(web_folder, filename)

    with open(file_path, "r", encoding="utf-8") as file:
        content = file.read()

    code_lines.append(f"const char {filename.replace('.', '_')}[] PROGMEM = R\"rawliteral(")
    code_lines.append(content)
    code_lines.append(")rawliteral\";")
    code_lines.append("")

code_lines.append("void setupStaticFiles(AsyncWebServer& server) {")

for filename in os.listdir(web_folder):
    content_type = ""
    if filename.endswith(".html"):
        content_type = "text/html"
    elif filename.endswith(".css"):
        content_type = "text/css"
    elif filename.endswith(".js"):
        content_type = "application/javascript"

    if filename == "index.html":
        code_lines.append(f'server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {{ handleCacheRequest(request, "{content_type}", index_html); }});')
        
    else: 
        code_lines.append(f'server.on("/{filename}", HTTP_GET, [](AsyncWebServerRequest* request) {{ handleCacheRequest(request, "{content_type}", {filename.replace(".", "_")}); }});')

code_lines.append("}")


os.makedirs(os.path.dirname(output_file), exist_ok=True)

with open(output_file, "w", encoding="utf-8") as header_file:
    header_file.write("\n".join(header_lines))

with open(output_file.replace(".h", ".cpp"), "w", encoding="utf-8") as code_file:
    code_file.write("\n".join(code_lines))

    