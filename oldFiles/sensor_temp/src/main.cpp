/*
 Code from: https://github.com/LilyGO/TTGO-T-Beam/blob/master/OLED_LoRa_Sender/OLED_LoRa_Sender.ino
*/

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SHT31.h>
#include "esp_adc_cal.h"
#include "ArduinoJson.h"
#include "WiFi.h"

#include "mesh.h"

#include "ESP32Time.h"
#include <Adafruit_ADS1X15.h> // bindet Wire.h für I2C mit ein

#define SDA1 21
#define SCL1 22
Adafruit_BMP280 bme; // I2C

#define SHT31_ADDRESS 0x44

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 50       /* Time ESP32 will go to sleep (in seconds) */

Adafruit_ADS1115 ads;

#define ADS_I2C_ADDR 0x48

struct 
{
  float battery;
  float panel;
} voltages;



SHT31 sht;
uint16_t status;

bool bme280Found = false;
bool sht31Found = false;

float temperature;
float humidity;
float pressure;

const int potPin = 36;

int analogBattery = A0;
int g_vref = 1100;

/*Prototypes*/
void printHeaterStatus(uint16_t status);
void readSht();
void adc_vref_init();
void get_battery();
unsigned long getTime();
void messureTask();

void setup()
{
  bme280Found = false;
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println();
  Serial.println("LoRa Sender Test");

  if (!bme.begin(0x76))
  {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    bme280Found = true;
  }

  if (!sht.begin(SHT31_ADDRESS))
  {
    Serial.println("Could not find a valid SHT31 sensor, check wiring!");
    sht31Found = true;
  }
  initMesh();
  Serial.println("init ok");
  if (sht31Found)
  {
    sht.setHeatTimeout(5); // heater timeout 30 seconds, just for demo.
  }
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
  }
  adc_vref_init();
  WiFi.mode(WIFI_OFF);
  btStop();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  timeSync();
  delay(2000);
  get_battery();
  messureTask();
  digitalWrite(LED_BUILTIN, LOW);
  delay(2000);
  esp_deep_sleep_start();
}

void messureTask()
{
  readSht();
  DynamicJsonDocument doc(1024);
  doc["cmd"] = "routeData";
  doc["source"] = WiFi.macAddress();
  doc["time"] = getTime();
  doc["battery"] = voltages.battery;
  doc["panel"] = voltages.panel;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  if (bme280Found)
  {
    doc["pressure"] = bme.readPressure();
  }
  String buffer;
  serializeJson(doc, buffer);
  sendData((char *)buffer.c_str(), buffer.length());
}
void loop() {}
void readSht()
{
  sht.read(false);
  temperature = sht.getTemperature();
  sht.heatOn();

  while (sht.isHeaterOn())
    ;
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
void adc_vref_init()
{
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize((adc_unit_t)ADC_UNIT_1, (adc_atten_t)ADC1_CHANNEL_6, (adc_bits_width_t)ADC_WIDTH_BIT_12, 1100, &adc_chars);
  // Check type of calibration value used to characterize ADC
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
  {
    Serial.printf("eFuse Vref:%u mV\n", adc_chars.vref);
    g_vref = adc_chars.vref;
  }
  else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
  {
    Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
  }
  else
  {
    Serial.println("Default Vref: 1100mV");
  }
}

void get_battery()
{
  int16_t adc0, adc1, adc2, adc3;
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  
  voltages.battery = ads.computeVolts(adc0);
  voltages.panel = ads.computeVolts(adc1);
}
unsigned long getTime()
{
  time_t now;
  time(&now);
  return now;
}