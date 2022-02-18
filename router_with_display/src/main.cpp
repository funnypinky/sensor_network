#include <Arduino.h>
#include "FS.h"
#include "FFat.h"
#include <esp_spi_flash.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <time.h>
#include "hmi.hpp"
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
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        //  downloadFile("http://openweathermap.org/img/wn/10d@2x.png", "/url.png");
    }
    lv_timer_t *clockLoop = lv_timer_create(loop_clock, 900, NULL);
    lv_timer_t *indoorLoop = lv_timer_create(loop_indoor_value, 9000, NULL);
}

void loop()
{
    lv_task_handler();
    delay(10);
    lv_tick_inc(10);
}

void downloadFile(String url, String filename)
{
    Serial.println("Downloading " + url + " and saving as " + filename);

    // wait for WiFi connection
    boolean isFirstCall = true;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");

    // configure server and url
    http.begin(url);

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    if (httpCode > 0)
    {
        // SPIFFS.remove(filename);
        File f = FFat.open(filename, "w+");
        if (!f)
        {
            Serial.println("file open failed");
            return;
        }
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK)
        {

            // get lenght of document (is -1 when Server sends no Content-Length header)
            int total = http.getSize();
            int len = total;
            // create buffer for read
            uint8_t buff[128] = {0};

            // get tcp stream
            WiFiClient *stream = http.getStreamPtr();

            // read all data from server
            while (http.connected() && (len > 0 || len == -1))
            {
                // get available data size
                size_t size = stream->available();

                if (size)
                {
                    // read up to 128 byte
                    int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                    // write it to Serial
                    f.write(buff, c);

                    if (len > 0)
                    {
                        len -= c;
                    }
                    isFirstCall = false;
                }
                delay(1);
            }

            Serial.println();
            Serial.print("[HTTP] connection closed or file end.\n");
        }
        f.close();
    }

    http.end();
}