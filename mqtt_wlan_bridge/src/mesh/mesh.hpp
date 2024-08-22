#pragma once

#include <vector>
#include <SPI.h> // include libraries
#include <LoRa.h>
#include <ArduinoJson.h>
#include <mqtt/mqtt.hpp>
#include "display/display.hpp"
#include "ESP32Time.h"
#include <Regexp.h>

#define CS 5   // LoRa radio chip select
#define RST 14 // LoRa radio reset
#define IRQ 26 // change for your board; must be a hardware interrupt pin
#define BAND 868E6

class Mesh
{
public:
    bool initMesh();
    void sendMessage(String outgoing);
    void onReceive(int packetSize);

private:
    ESP32Time rtc;
    void parseString(String toParse);
    void sendTimeSync();
    unsigned long getTime();
    std::vector<String> splitString(String string, char delim);
    };