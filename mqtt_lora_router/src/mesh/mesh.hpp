#pragma once

#include <SPI.h> // include libraries
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
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

    static void sendTimeSync(uint8_t to);
    static void LoRa_sendMessage(char *message, int len, uint8_t to);
    static void route2Mqtt(const char *topic, const char *message);
    static unsigned long getTime();
    static void taskRec();

private:
    static void onTxDone();
};
