#pragma once

#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <LoRa.h>

#define SCK 5   // GPIO5 -- SX127x's SCK
#define MISO 19 // GPIO19 -- SX127x's MISO
#define MOSI 27 // GPIO27 -- SX127x's MOSI
#define SS 18   // GPIO18 -- SX127x's CS
#define RST 14  // GPIO14 -- SX127x's RESET
#define DI0 26  // GPIO26 -- SX127x's IRQ (Interrupt Request)
#define BAND 868E6

void initMesh();
void updateRoutingTable();
void getRouteInfoString(char *p, size_t len);
void printNodeInfo(uint8_t node, char *s);
void collectRouteData();
void sendData(String data);