#pragma once

#include <SPI.h> // include libraries
#include <LoRa.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <sys/time.h>
#include <vector>
#include "WiFi.h"
#include <ESP32Time.h>

#define CS 5   // LoRa radio chip select
#define RST 14 // LoRa radio reset
#define IRQ 26 // change for your board; must be a hardware interrupt pin
#define BAND 868E6

class Mesh
{

public:
    bool initMesh();
    void sendMessage(String outgoing);
    String onReceive(int packetSize);
    void sleep();
        
private:
    void parseString(String toParse);
    };