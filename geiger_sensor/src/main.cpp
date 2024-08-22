#include <Arduino.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <ESP32Time.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>
#include "mqtt/mqtt.hpp"
#include <Ethernet.h>

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

const float factorCPMtouSv = 0.00114516129032258F;

const int watchdogTimeoutMicros = 40000000L;
hw_timer_t *watchdogTimer = NULL;

// Prototyps
uint16_t takeSampleNoSleep();
uint16_t takeSampleLowPower();
void calib();
void postFloatValue (float measurement, int digits, String sensorId);
void waitForServerResponse ();

ESP32Time rtc(0);

//senseBox ID
#define SENSEBOX_ID "663a337e6e496b0007d6aa7c"

//Sensor IDs
// Background radiation - undefined
#define SENSOR1_ID "663a337e6e496b0007d6aa7d"

//Ethernet-Parameter
char server[] = "ingress.opensensemap.org";
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// Diese IP Adresse nutzen falls DHCP nicht möglich
IPAddress myIP(192, 168, 0, 42);
WiFiClient clientWifi;
HTTPClient http;

//Messparameter
const unsigned int postingInterval = 60000; //Uploadintervall in Millisekunden


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
  //calib();
  if (connect())
  {
   Serial.println("Node connected");
  }
}

void calib() {
  for (int i = 0; i< 10; i++) {
    const uint16_t pulses = takeSampleNoSleep();
    Serial.printf("%i. measurement: %i\n", i+1, pulses);
    esp_task_wdt_reset();
  }
}

void loop() {
	JsonDocument doc;
	const uint16_t pulses = takeSampleNoSleep();
	doc["time"] = rtc.getLocalEpoch();
  doc["src"] = WiFi.macAddress();
  doc["values"]["CPM"] = pulses;
  doc["values"]["ambient_dose_rate"] = pulses*factorCPMtouSv;
	String buf;
    serializeJson(doc, buf);
    Serial.println(buf);
    String topic = "sensor/";
    topic += WiFi.macAddress(); 
    publish(topic.c_str(), buf.c_str());
    buf.clear();
    postFloatValue(pulses*factorCPMtouSv, 4, SENSOR1_ID);
    esp_task_wdt_reset();
}

void postFloatValue (float measurement, int digits, String sensorId) {
  //Float zu String konvertieren
  char obs[10];
  dtostrf(measurement, 5, digits, obs);
  //Json erstellen
  String jsonValue = "{\"value\":";
  jsonValue += obs;
  jsonValue += "}";
  //Mit OSeM Server verbinden und POST Operation durchführen
  Serial.println("-------------------------------------");
  Serial.print("Connecting to OSeM Server...");
  if(WiFi.status()== WL_CONNECTED){
    
      // Your Domain name with URL path or IP address with path
      http.begin(clientWifi, server);
    if (clientWifi.connect(server, 80))  {
    Serial.println("connected!");
    Serial.println("-------------------------------------");
    //HTTP Header aufbauen
    clientWifi.print("POST /boxes/"); 
    clientWifi.print(SENSEBOX_ID); 
    clientWifi.print("/"); 
    clientWifi.print(sensorId); 
    clientWifi.println(" HTTP/1.1");
    clientWifi.print("Host:");
    clientWifi.println(server);
    clientWifi.print("Authorization:");
    clientWifi.println("3d400066fe31971d182862117db90a624bbbecd6dd5ca8e7ab2fd20f55ae2aae");
    clientWifi.println("Content-Type: application/json");
    clientWifi.println("Connection: close");
    clientWifi.print("Content-Length: "); 
    clientWifi.println(jsonValue.length());
    clientWifi.println();
    //Daten senden
    clientWifi.println(jsonValue);
  } else {
    Serial.println("failed!");
    Serial.println("-------------------------------------");
  }
  }
  //Antwort von Server im seriellen Monitor anzeigen
  waitForServerResponse();
}

void waitForServerResponse () {
  //Ankommende Bytes ausgeben
  boolean repeat = true;
  do {
    if (clientWifi.available()) {
      char c = clientWifi.read();
      Serial.print(c);
    }
    //Verbindung beenden
    if (!clientWifi.connected()) {
      Serial.println();
      Serial.println("--------------");
      Serial.println("Disconnecting.");
      Serial.println("--------------");
      clientWifi.stop();
      repeat = false;
    }
  } while (repeat);
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