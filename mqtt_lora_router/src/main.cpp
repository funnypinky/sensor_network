#include <Arduino.h>
#include "mesh/mesh.hpp"
#include <U8g2lib.h>
#include "wifi.hpp"
#include "mqtt/mqtt.hpp"
#include <arduino-timer.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2);

// Create a U8g2log object
U8G2LOG u8g2log;

Mesh mesh;

unsigned long last;

// assume 4x6 font, define width and height
#define U8LOG_WIDTH 32
#define U8LOG_HEIGHT 10

// allocate memory
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];
void setup()
{
  Serial.begin(9600);
  Serial.println(ESP.getFreeHeap());
  u8g2.begin();
  u8g2.setFont(u8g2_font_tom_thumb_4x6_mf);
  u8g2log.begin(u8g2, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8g2log.setLineHeightOffset(0);
  while (!setupWifi(&u8g2log))
  {
    ;
  }

  while (!Serial)
    ; // Wait for serial port to be available
  if (!mesh.initMesh(&u8g2log))
  {
    u8g2log.println("init failed");
  }
  if (connect())
  {
    u8g2log.println("MQTT connected");
  }
  else
  {
    reconnect();
  }
  u8g2log.println("init successfully");
  Serial.println(ESP.getFreeHeap());
  last = millis();
}

void loop()
{
  mesh.taskRec();
  if (millis() - last >= 1000)
  {
    taskSendFifo();
    last = millis();
  }
}