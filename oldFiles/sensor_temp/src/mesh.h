#pragma once

#include <Wire.h>
#include <SPI.h> // include libraries
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

#define SCK 18   // GPIO5 -- SX127x's SCK
#define MISO 19 // GPIO19 -- SX127x's MISO
#define MOSI 23 // GPIO27 -- SX127x's MOSI
#define SS 5   // GPIO18 -- SX127x's CS
#define RST 14  // GPIO14 -- SX127x's RESET
#define DI0 26  // GPIO26 -- SX127x's IRQ (Interrupt Request)
#define BAND 868E6

void initMesh();
void updateRoutingTable();
void getRouteInfoString(char *p, size_t len);
void printNodeInfo(uint8_t node, char *s);
void collectRouteData();
void LoRa_sendMessage(char *message, int len);
void sendData(char *sendData, int len);
void timeSync();
void taskReceive();