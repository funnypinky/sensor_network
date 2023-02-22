#include <Arduino.h>
#include "FS.h"
#include "FFat.h"
#include <esp_spi_flash.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <time.h>
#include "hmi/hmi.hpp"
#include "sensor.hpp"
#include "lvgl.h"

const char *ssid = "CastleWLAN2,4GHz";
const char *password = "cleverstreet401";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

WiFiMulti _wifiMulti;

void downloadFile(String url, String filename);

void setup()
{
    Serial.begin(9600);
    Serial.println("Start Node");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    WiFi.begin(ssid, password);
    FFat.begin();
    if (!initSensor())
    {
        Serial.println("Sensor init not successfully");
    }
    setupHMI();
}

void loop()
{
    lv_timer_handler();
    lv_tick_inc(5);
    delay(5);
}
