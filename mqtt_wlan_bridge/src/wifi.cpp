#include "wifi.hpp"

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

bool setupWifi()
{
    WiFiManager manager;

    //u8g2log->println("Setup Wifi!");
    //u8g2log->println("AP: MQTT_LoRa_Gateway");
    bool success = manager.autoConnect("MQTT_LoRa_Gateway");
    if (!success)
    {
        //u8g2log->println("Failed to connect");
        return false;
    }
    else
    {
        //u8g2log->println("Connected");
        IPAddress ip = WiFi.localIP();
        //u8g2log->printf("IP: %s\n", ip.toString().c_str());
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        return true;
    }
}