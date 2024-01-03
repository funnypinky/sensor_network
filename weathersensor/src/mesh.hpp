#pragma once

#include <SPI.h> // include libraries
#include <RadioLib.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <sys/time.h>
#include <vector>
#include "WiFi.h"
#include <ESP32Time.h>

#define CS 5   // LoRa radio chip select
#define RST 14 // LoRa radio reset
#define IRQ 26 // change for your board; must be a hardware interrupt pin
#define FREQUENCY 868.0
class Mesh
{

public:
    bool initMesh();
    void sendMessage(String outgoing);
    String onReceive(int packetSize);
    void sleep();
    void syncTime();
    std::vector<String> splitString(String string, char delim);

private:
    void parseString(String toParse);
    SX1276 loraModule = new Module(1, CS, RST, IRQ);
    ESP32Time rtc;
};