#pragma once

#include <SPI.h> // include libraries
#include <LoRa.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <sys/time.h>

#define CS 5   // LoRa radio chip select
#define RST 14 // LoRa radio reset
#define IRQ 26 // change for your board; must be a hardware interrupt pin
#define BAND 868E6

class Mesh
{

public:
    bool initMesh();
    void sendMessage(String outgoing);
    bool onReceive(int packetSize);

private:
    void parseString(String toParse);
};