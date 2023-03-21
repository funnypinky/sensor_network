#include <Adafruit_Sensor.h>
#include <SHT31.h>
#include <MS5611.h>
#include "esp_adc_cal.h"
#include "WiFi.h"
#include <ESP32Time.h>
#include <esp_task_wdt.h>
#include <vector>
#include "mesh.hpp"

#include <Adafruit_ADS1X15.h> // bindet Wire.h für I2C mit ein
#include <cmath>
#include "math.h"

#define SDA1 21
#define SCL1 22

MS5611 ms5611(0x76);

#define SHT31_ADDRESS 0x44

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 120      /* Time ESP32 will go to sleep (in seconds) */
#define WDT_TIMEOUT 150

Adafruit_ADS1115 ads;

#define ADS_I2C_ADDR 0x48

struct
{
  float battery;
  float panel;
} voltages;

SHT31 sht;
uint16_t status;

bool sht31Found = false;

float temperature;
float humidity;
float pressure;

const int potPin = 36;

int analogBattery = A0;
int g_vref = 1100;

ESP32Time rtc;

Mesh mesh;

/*Prototypes*/
void printHeaterStatus(uint16_t status);
void readSht();
void get_battery();
unsigned long getTime();
void messureTask();
float round(float value, int precision);
void syncTime();
std::vector<String> splitString(String string, char delim);

void setup()
{
  //Disable all unused feature
  WiFi.mode(WIFI_OFF);
  btStop();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(25, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(25, HIGH);
  Serial.begin(9600);
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);
  Serial.println();
  Serial.println("Temperature sensor measuring start");
  mesh.initMesh();
  Serial.println("init ok");
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  syncTime();
  messureTask();
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(25, LOW);
  esp_task_wdt_reset();
  esp_deep_sleep_start();
}

void messureTask()
{

  String txStr;
  txStr += "data;";
  txStr += WiFi.macAddress()+";";

  if (ads.begin())
  {
    get_battery();
    txStr +=String(voltages.battery) +";";
    txStr += String(voltages.panel)+";";
  }
  if (sht.begin())
  {
    readSht();

    txStr += String(temperature)+";";
    txStr += String(humidity)+";";
  } else {
    txStr += ";";
    txStr += ";";
  }
  if (ms5611.begin())
  {
    ms5611.setOversampling(OSR_ULTRA_HIGH);
    ms5611.read();
    txStr += String(ms5611.getPressure())+";";
  }
  txStr += String(rtc.getLocalEpoch())+";";
  Serial.println(txStr);
  mesh.sendMessage(txStr);
}
void loop() {}

void readSht()
{
  sht.read(false);
  temperature = sht.getTemperature();
  sht.heatOn();

  while (sht.isHeaterOn())
    ;

  sht.heatOff();
  sht.read(false);
  humidity = sht.getHumidity();
}
void printHeaterStatus(uint16_t status)
{
  Serial.print(millis());
  Serial.print("\tHEATER: ");
  if (status & SHT31_STATUS_HEATER_ON)
  {
    Serial.println("ON");
  }
  else
  {
    Serial.println("OFF");
  }
}

void get_battery()
{
  int16_t adc0, adc2;
  ads.setGain(GAIN_TWOTHIRDS); // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  adc0 = ads.readADC_SingleEnded(0);
  adc2 = ads.readADC_SingleEnded(2);

  voltages.battery = ads.computeVolts(adc0);
  voltages.panel = ads.computeVolts(adc2);
}
unsigned long getTime()
{
  time_t now;
  time(&now);
  return now;
}

float round(float value, int precision)
{
  float multiplier = std::pow(10, precision);
  return (float)((int)(value * multiplier + 0.5f)) / multiplier;
}

void syncTime() {
  bool timeSync = false;
  String reqTime;
  reqTime += "reqTime;";
  reqTime += WiFi.macAddress()+";";
  mesh.sendMessage(reqTime);
  while(!timeSync) {
    String answer = mesh.onReceive(LoRa.parsePacket());
    if(answer != "") {
      std::vector<String> measVec = splitString(answer,';');
      if(measVec[0] == "repTime") {
        rtc.setTime(atol(measVec[2].c_str()));
        Serial.println(rtc.getTime());
      }
      timeSync = true;
    }
  }
}

std::vector<String> splitString(String string, char delim) 
{
    String temp = string.substring(0, string.length());
    std::vector<String> returnVector;
    int index = string.indexOf(delim);
    while (index != -1) {
        returnVector.push_back(temp.substring(0,index));
        temp = temp.substring(index+1,string.length());
        index = temp.indexOf(delim);
    }
    return returnVector;
}