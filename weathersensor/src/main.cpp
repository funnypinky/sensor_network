#include <Adafruit_Sensor.h>
#include <SHT31.h>
#include <MS5611.h>
#include "esp_adc_cal.h"
#include "WiFi.h"
#include <esp_task_wdt.h>
#include <vector>
#include "config_lorawan.h"
#include <esp32-hal-adc.h>

#include <cmath>
#include "math.h"
#include <RadioLib.h>
#include <Preferences.h>
Preferences store;

RTC_DATA_ATTR uint16_t bootCount = 0;
RTC_DATA_ATTR uint16_t bootCountSinceUnsuccessfulJoin = 0;
RTC_DATA_ATTR uint8_t LWsession[RADIOLIB_LORAWAN_SESSION_BUF_SIZE];

// I2C PINS
#define SDA1 21
#define SCL1 22

// ADC-PINS
#define ADC_BAT 36
#define ADC_PANEL 39

MS5611 ms5611(0x77);

#define SHT31_ADDRESS 0x44

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

int16_t state = 0;

/*Prototypes*/
int16_t lwActivate();
void gotoSleep(uint32_t seconds);
void printHeaterStatus(uint16_t status);
void readSht();
void get_battery();
unsigned long getTime();
void messureTask();
float round(float value, int precision);
std::vector<String> splitString(String string, char delim);

void setup()
{
  // Disable all unused feature
  WiFi.mode(WIFI_OFF);
  btStop();
  Serial.begin(9600);
  Wire.begin(SDA1, SCL1);
  Serial.println(F("Initalise the radio"));
  state = radio.begin();
  debug(state != RADIOLIB_ERR_NONE, F("Initalise radio failed"), state, true);

  // activate node by restoring session or otherwise joining the network
  state = lwActivate();

  messureTask();

  uint8_t *persist = node.getBufferSession();
  memcpy(LWsession, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);
  gotoSleep(uplinkIntervalSeconds);
}

void messureTask()
{
  uint8_t payload[8];
  get_battery();
  node.setDeviceStatus(voltages.battery);
  if (sht.begin())
  {
    readSht();
    int tmp = ((int)(temperature * 100)) + 27315;
    payload[0] = tmp >> 8;
    payload[1] = tmp;
    int hum = (int)(humidity * 2);
    payload[2] = hum;
  }
  if (ms5611.begin())
  {
    ms5611.setOversampling(OSR_ULTRA_HIGH);
    ms5611.read();
    int pre = (int)(ms5611.getPressure() * 10);
    payload[3] = pre >> 8;
    payload[4] = pre;
  }

  node.uplink(payload,4, true);
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

float round(float value, int precision)
{
  float multiplier = std::pow(10, precision);
  return (float)((int)(value * multiplier + 0.5f)) / multiplier;
}


void gotoSleep(uint32_t seconds)
{
  esp_sleep_enable_timer_wakeup(seconds * 1000UL * 1000UL); // function uses uS
  Serial.println(F("Sleeping\n"));
  Serial.flush();

  esp_deep_sleep_start();

  // if this appears in the serial debug, we didn't go to sleep!
  // so take defensive action so we don't continually uplink
  Serial.println(F("\n\n### Sleep failed, delay of 5 minutes & then restart ###\n"));
  delay(5UL * 60UL * 1000UL);
  ESP.restart();
}

int16_t lwActivate()
{
  int16_t state = RADIOLIB_ERR_UNKNOWN;

  // setup the OTAA session information
  node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);

  Serial.println(F("Recalling LoRaWAN nonces & session"));
  // ##### setup the flash storage
  store.begin("radiolib");
  // ##### if we have previously saved nonces, restore them and try to restore session as well
  if (store.isKey("nonces"))
  {
    uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];                   // create somewhere to store nonces
    store.getBytes("nonces", buffer, RADIOLIB_LORAWAN_NONCES_BUF_SIZE); // get them from the store
    state = node.setBufferNonces(buffer);                               // send them to LoRaWAN
    debug(state != RADIOLIB_ERR_NONE, F("Restoring nonces buffer failed"), state, false);

    // recall session from RTC deep-sleep preserved variable
    state = node.setBufferSession(LWsession); // send them to LoRaWAN stack

    // if we have booted more than once we should have a session to restore, so report any failure
    // otherwise no point saying there's been a failure when it was bound to fail with an empty LWsession var.
    debug((state != RADIOLIB_ERR_NONE) && (bootCount > 1), F("Restoring session buffer failed"), state, false);

    // if Nonces and Session restored successfully, activation is just a formality
    // moreover, Nonces didn't change so no need to re-save them
    if (state == RADIOLIB_ERR_NONE)
    {
      Serial.println(F("Succesfully restored session - now activating"));
      state = node.activateOTAA();
      debug((state != RADIOLIB_LORAWAN_SESSION_RESTORED), F("Failed to activate restored session"), state, true);

      // ##### close the store before returning
      store.end();
      return (state);
    }
  }
  else
  { // store has no key "nonces"
    Serial.println(F("No Nonces saved - starting fresh."));
  }

  // if we got here, there was no session to restore, so start trying to join
  state = RADIOLIB_ERR_NETWORK_NOT_JOINED;
  while (state != RADIOLIB_LORAWAN_NEW_SESSION)
  {
    Serial.println(F("Join ('login') to the LoRaWAN Network"));
    state = node.activateOTAA();

    // ##### save the join counters (nonces) to permanent store
    Serial.println(F("Saving nonces to flash"));
    uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];                   // create somewhere to store nonces
    uint8_t *persist = node.getBufferNonces();                          // get pointer to nonces
    memcpy(buffer, persist, RADIOLIB_LORAWAN_NONCES_BUF_SIZE);          // copy in to buffer
    store.putBytes("nonces", buffer, RADIOLIB_LORAWAN_NONCES_BUF_SIZE); // send them to the store

    // we'll save the session after an uplink

    if (state != RADIOLIB_LORAWAN_NEW_SESSION)
    {
      Serial.print(F("Join failed: "));
      Serial.println(state);

      // how long to wait before join attempts. This is an interim solution pending
      // implementation of TS001 LoRaWAN Specification section #7 - this doc applies to v1.0.4 & v1.1
      // it sleeps for longer & longer durations to give time for any gateway issues to resolve
      // or whatever is interfering with the device <-> gateway airwaves.
      uint32_t sleepForSeconds = min((bootCountSinceUnsuccessfulJoin++ + 1UL) * 60UL, 3UL * 60UL);
      Serial.print(F("Boots since unsuccessful join: "));
      Serial.println(bootCountSinceUnsuccessfulJoin);
      Serial.print(F("Retrying join in "));
      Serial.print(sleepForSeconds);
      Serial.println(F(" seconds"));

      gotoSleep(sleepForSeconds);

    } // if activateOTAA state
  } // while join

  Serial.println(F("Joined"));

  // reset the failed join count
  bootCountSinceUnsuccessfulJoin = 0;

  delay(1000); // hold off off hitting the airwaves again too soon - an issue in the US

  // ##### close the store
  store.end();
  return (state);
}
