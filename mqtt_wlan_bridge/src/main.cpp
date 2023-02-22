#include <Arduino.h>
#include "mesh/mesh.hpp"
#include "display/display.hpp"
#include "mqtt/mqtt.hpp"
#include <TaskScheduler.h>
#include "wifi.hpp"

Mesh mesh;

unsigned long last;

void t1Callback();
void t2Callback();
Scheduler runner;
Task t1(10, TASK_FOREVER, &t1Callback);
//8,64e+7
Task t2(1000, TASK_FOREVER, &t2Callback);
void setup()
{
  Serial.begin(9600);
  runner.init();
  runner.addTask(t1);
  initDisplay();
  while (!setupWifi())
  {
    ;
  }
  
  while (!Serial)
    ; // Wait for serial port to be available
  if (!mesh.initMesh())
  {
    
  }
  if (connect())
  {
    
  }
  else
  {
    reconnect();
  }
  last = millis();
  writeInfo(5,"init complete");
  t2.enable();
  t1.enable();
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

void loop()
{
  runner.execute();
}