#include "sensor.hpp"

Adafruit_BME280 bmp; // I2C

boolean initSensor() {
    boolean stat = bmp.begin(0x76);
    return stat;
}

float getTemperatur() {
    return bmp.readTemperature();
}

float getPressure() {
    return bmp.readPressure();
}

float getHumidity() {
    return bmp.readHumidity();
}
