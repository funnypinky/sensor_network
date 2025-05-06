#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "STM32LowPower.h"
#include "config_lorawan.h"
#include "EEPROM.h"

#define LED PB5
#define BME280_SDA PA15
#define BME280_SCL PB15
#define BME280_VCC PA3

Adafruit_BME280 bmp; // I2C

int16_t lwActivate(void);

uint8_t LWsession[RADIOLIB_LORAWAN_SESSION_BUF_SIZE];
uint16_t storedAddress = 0;
const int WRITTEN_SIGNATURE = 0xBEEFDEED;

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(BME280_VCC, OUTPUT);
  digitalWrite(BME280_VCC, LOW); // turn on the BME280 VCC
  delay(100);                    // wait for the BME280 to power up
  Wire.setSDA(BME280_SDA);
  Wire.setSCL(BME280_SCL);
  Wire.begin();
  unsigned status;
  // default settings
  status = bmp.begin(0x76);
  if (!status)
  {
    digitalWrite(LED, HIGH); // turn on the LED to indicate error
  }
  bmp.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1,   // temperature
                  Adafruit_BME280::SAMPLING_NONE, // pressure
                  Adafruit_BME280::SAMPLING_X1,   // humidity
                  Adafruit_BME280::FILTER_OFF);
  LowPower.begin();
  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);

  // initialize STM32WL with default settings, except frequency
  Serial.print(F("[STM32WL] Initializing ... "));
  int state = radio.begin(868.0);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("success!"));

  }
  else
  {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true)
    {
      delay(10);
    }
  }
  state = lwActivate();
}

// the loop function runs over and over again forever

void loop()
{ // wait for a second
  if (bmp.takeForcedMeasurement())
  {
    uint8_t payload[4];
    float temperature = bmp.readTemperature();
    float humidity = bmp.readHumidity();
    
    int tmp = ((int)(temperature * 100)) + 5000;
    payload[0] = highByte(tmp);
    payload[1] = lowByte(tmp);
    int hum = (int)(humidity * 10);
    payload[2] = highByte(hum);
    payload[3] = lowByte(hum);
    node.sendReceive(payload, sizeof(payload));
  } 
    
  digitalWrite(LED, HIGH);
  // turn off the LED to indicate success
  LowPower.deepSleep(60000);
  digitalWrite(LED, LOW);
}

int16_t lwActivate()
{
  int16_t state;
  int signature;

  state = node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);
  EEPROM.get(storedAddress, signature);
  if (signature == WRITTEN_SIGNATURE)
  {
    uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];
    EEPROM.get(storedAddress + sizeof(signature), buffer);
    state = node.setBufferNonces(buffer);
    state = node.setBufferSession(LWsession);
    if (state == RADIOLIB_ERR_NONE)
    {
      state = node.activateOTAA();
      return (state);
    }
  } else {
    state = RADIOLIB_ERR_NETWORK_NOT_JOINED;
    while (state != RADIOLIB_LORAWAN_NEW_SESSION)
    {
      state = node.activateOTAA();
  
      // ##### save the join counters (nonces) to permanent store
      
      uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];                   // create somewhere to store nonces
      uint8_t *persist = node.getBufferNonces();
      memcpy(buffer, persist, RADIOLIB_LORAWAN_NONCES_BUF_SIZE);                            // get pointer to nonces
      EEPROM.put(storedAddress, WRITTEN_SIGNATURE);
      EEPROM.put(storedAddress + sizeof(signature), buffer);
  }
  return (state);
  }
}