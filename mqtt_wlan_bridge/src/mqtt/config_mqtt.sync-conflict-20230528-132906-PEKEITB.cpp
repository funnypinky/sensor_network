#include <WiFi.h>
#include <AsyncTCP.h>
#include "FS.h"
#include "FFat.h"

#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void startConfigServer()
{
    FFat.begin();
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(FFat, "/index.html", String()); });

    // Route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(FFat, "/style.css", "text/css"); });
}