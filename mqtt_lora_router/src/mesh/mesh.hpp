#pragma once

#include <SPI.h> // include libraries
#include <LoRa.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <time.h>
#include "mqtt/mqtt.hpp"

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

class Mesh
{
public:
    void startMesh(void *parameter);
    bool initMesh(U8G2LOG *u8g2log);
    static void LoRa_rxMode();
    static void LoRa_txMode();

    static void sendTimeSync();
    static void LoRa_sendMessage(String message);
    static void route2Mqtt(String topic, String message);
    static unsigned long getTime();

private:
    static void onReceive(int packetSize);
    static void onTxDone();
};
