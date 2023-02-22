#pragma once

#include <Arduino.h>

class ConfigMQTT
{
public:
    String mqtt_broker = "10.72.72.124";
    IPAddress mqtt_ip = IPAddress(10, 72, 72, 124);
    String user = "broker";
    String pw = "Rechner123";
    uint16_t port = 1883;
};