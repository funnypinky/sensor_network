#include <Adafruit_Sensor.h>
#include <SHT31.h>
#include <MS5611.h>
#include "esp_adc_cal.h"
#include "WiFi.h"
#include <ESP32Time.h>
#include <esp_task_wdt.h>
#include <vector>
#include "mesh.hpp"
#include <esp32-hal-adc.h>

#include <cmath>
#include "math.h"

// I2C PINS
#define SDA1 21
#define SCL1 22

// ADC-PINS
#define ADC_BAT 36
#define ADC_PANEL 39

MS5611 ms5611(0x77);

#define SHT31_ADDRESS 0x44

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 110      /* Time ESP32 will go to sleep (in seconds) */
#define WDT_TIMEOUT 150

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
RTC_DATA_ATTR long lastSync = 0;

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
  // Disable all unused feature
  WiFi.mode(WIFI_OFF);
  btStop();
  Serial.begin(9600);
  Wire.begin(SDA1, SCL1);

  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);

  Serial.println();
  Serial.println("Temperature sensor measuring start");

  if (mesh.initMesh())
  {
  }
  Serial.println(lastSync);
  if ((lastSync == 0) || ((rtc.getLocalEpoch() - lastSync) > 43200))
  {
    syncTime();
    lastSync = rtc.getLocalEpoch();
    Serial.println("time sync");
  }
  Serial.println("init ok");

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  messureTask();
  delay(2000);
  mesh.sleep();
  
  esp_task_wdt_reset();
  esp_deep_sleep_start();
}

void messureTask()
{
  get_battery();
  String txStr;
  txStr += "data;";
  txStr += WiFi.macAddress() + ";";
  txStr += String(voltages.battery) + ";";
  txStr += String(voltages.panel) + ";";
  if (sht.begin())
  {
    readSht();
    txStr += String(temperature) + ";";
    txStr += String(humidity) + ";";
  }
  else
  {
    txStr += ";";
    txStr += ";";
  }
  if (ms5611.begin())
  {
    ms5611.setOversampling(OSR_ULTRA_HIGH);
    ms5611.read();
    txStr += String(ms5611.getPressure()) + ";";
  }
  else
  {
    txStr += ";";
  }
  txStr += String(rtc.getLocalEpoch()) + ";";
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

uint32_t readADC_cal(int ADC_Raw)
{
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_12Bit, 1100, &adc_chars);
  return (esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}

void get_battery()
{
  analogSetClockDiv(32); // Try to increase

  voltages.battery = (readADC_cal(analogRead(ADC_BAT)) / 0.192) / 1000;
  voltages.panel = (readADC_cal(analogRead(ADC_PANEL)) / 0.05475) / 1000;
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

void syncTime()
{
  bool timeSync = false;
  String reqTime;
  reqTime += "reqTime;";
  reqTime += WiFi.macAddress() + ";";
  mesh.sendMessage(reqTime);
  while (!timeSync)
  {
    String answer = mesh.onReceive(LoRa.parsePacket());
    if (answer != "")
    {
      std::vector<String> measVec = splitString(answer, ';');
      if (measVec[0] == "repTime")
      {
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
  while (index != -1)
  {
    returnVector.push_back(temp.substring(0, index));
    temp = temp.substring(index + 1, string.length());
    index = temp.indexOf(delim);
  }
  return returnVector;
}