#include <Arduino.h>
#include <WiFiManager.h>
#include <WiFi.h> 
#include <time.h>
#include <ESP32Time.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>
#include "mqtt/mqtt.hpp"

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 120      /* Time ESP32 will go to sleep (in seconds) */
#define WDT_TIMEOUT 150

#define PULSE_PIN 14
#define PULSE_GPIO GPIO_NUM_14

#define MY_NTP_SERVER "de.pool.ntp.org"

// choose your time zone from this list
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"

// Pulses counted by interrupt (while CPU is awake)
volatile uint16_t intPulseCount = 0;
// Pulses counted during ESP light sleep
volatile uint16_t pulseCount = 0;

// Sample duration in µs
const uint32_t sampleMicros = 60 * 1000000;
// Absolute sample interval start micros
uint32_t sampleStart = 0;

const int16_t ingestInterval = 60;
int16_t ingestCountdown;

const float factorCPMtouSv = 0.0011508683825068;

const int watchdogTimeoutMicros = 40000000L;
hw_timer_t *watchdogTimer = NULL;

// Prototyps
uint16_t takeSampleNoSleep();
uint16_t takeSampleLowPower();

ESP32Time rtc(0);

void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println("Start geiger counter");
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);
  pinMode(PULSE_PIN, INPUT);
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("AutoConnectAP");
  configTime(0, 0, MY_NTP_SERVER);  // 0, 0 because we will use TZ in the next line
  setenv("TZ", MY_TZ, 1);            // Set environment variable with your time zone
  tzset();
  delay(2000);
  Serial.println(rtc.getTimeDate());
  if (connect())
  {
   Serial.println("Node connected");
  }
}

void loop() {
	DynamicJsonDocument doc(512);
	const uint16_t pulses = takeSampleNoSleep();
	doc["time"] = rtc.getLocalEpoch();
    doc["src"] = WiFi.macAddress();
    doc["values"]["CPM"] = pulses;
    doc["values"]["ambient_dose_rate"] = pulses*factorCPMtouSv;
	String buf;
    serializeJson(doc, buf);
    Serial.println(buf);
    publish(WiFi.macAddress().c_str(), buf.c_str());
    buf.clear();
    doc.clear();
    esp_task_wdt_reset();
}

void pulse()
{
	++intPulseCount;
}

uint32_t calcRemainingWait()
{
	const uint32_t remaining = sampleMicros - (micros() - sampleStart);
	return remaining > sampleMicros ? 0 : remaining;
}

uint16_t takeSampleNoSleep()
{
	attachInterrupt(PULSE_PIN, pulse, RISING);

	int32_t remainingWait = calcRemainingWait();
	delayMicroseconds(remainingWait);
	sampleStart = micros();
	noInterrupts();
	const int16_t pulses = intPulseCount;
	intPulseCount = 0;
	interrupts();

	return pulses;
}