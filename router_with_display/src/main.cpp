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

void loop_indoor_value(lv_timer_t *timer)
{
    updateIndoorValues(getPressure() / 100, getTemperatur(), getHumidity());
}

void loop_clock(lv_timer_t *timer)
{
    updateTime();
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Start Node");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    WiFi.begin(ssid, password);
    FFat.begin();
    initSensor();
    setupHMI();
    lv_timer_t *clockLoop = lv_timer_create(loop_clock, 900, NULL);
    lv_timer_t *indoorLoop = lv_timer_create(loop_indoor_value, 9000, NULL);
}

void loop()
{
    lv_task_handler();
    delay(10);
    lv_tick_inc(10);
}
