#include <Arduino.h>
#include "mesh/mesh.hpp"
#include "display/display.hpp"
#include "mqtt/mqtt.hpp"
#include <TaskScheduler.h>
#include "wifi.hpp"
#include <ESP32Time.h>

Mesh mesh;

ESP32Time rtc(3600);

unsigned long last;

void t1Callback();
void t2Callback();
void t3Callback();
Scheduler runner;
Task t1(10, TASK_FOREVER, &t1Callback);
//8,64e+7
Task t2(1000, TASK_FOREVER, &t2Callback);
Task t3(900, TASK_FOREVER, &t3Callback);
void setup()
{
  Serial.begin(9600);
  runner.init();
  runner.addTask(t1);
  runner.addTask(t3);
  initDisplay();
  writeInfo(2,"Start node");
  
  if(!setupWifi()){
    writeInfo(2,"Wifi not connected");
  }
  configTime(3600,3600, "de.pool.ntp.org");
  
  while (!Serial)
    ; // Wait for serial port to be available
  if (!mesh.initMesh())
  {
    writeInfo(3,"Mesh not connected");
  }
  if (connect())
  {
    writeInfo(4,"Node connected");
  }
  else
  {
    writeInfo(4,"Node not connected");
    reconnect();
  }
  last = millis();
  writeInfo(2,"Status: running");
  //t2.enable();
  //t1.enable();
  t3.enable();
}
void t1Callback()
{
  mesh.onReceive(LoRa.parsePacket());
}
void t2Callback()
{
  Serial.println("restart ESP");
  ESP.restart();
}
void t3Callback()
{
  writeDateTime(rtc.getTime("%d.%b").c_str(), rtc.getTime("%H:%M").c_str());
}

void loop()
{
  runner.execute();
}