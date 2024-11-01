#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Wire.h>
#include "hyt221.h"

#define HYT221_ADDR 0x28
// #define SCALE_MAX 0b100000000000000
#define SCALE_MAX 16384.0
#define TEMP_OFFSET  40.0
#define TEMP_SCALE 165.0
#define HUM_SCALE 100.0

//
// HYT221 
//
// !!! Blocking code
//

HYT221::HYT221(int I2Cadr){
    address = I2Cadr;
}

uint8_t HYT221::begin(void) {
    return 1;
}

uint8_t HYT221::read( void ) {
    Wire.beginTransmission(address);
    Wire.write((byte)0x00);
    Wire.available();
    int Ack = Wire.read(); // receive a byte
    
    // DEBUG
    #if HYT_DEBUG
        Serial.print("ACK: ");
        Serial.println(Ack);
    #endif
    
    Wire.endTransmission();
    
    // DEBUG ////////////////
    //request 4 bytes
    #if HYT_DEBUG
        Wire.requestFrom(address, 4);

        Wire.available();

        int a1 = Wire.read(); // receive a byte
        int a2 = Wire.read(); // receive a byte
        int a3 = Wire.read(); // receive a byte
        int a4 = Wire.read(); // receive a byte
    #endif
    ////////////////////////////////
    
    // delay inteval !!! blocking MCU
    delay(100);

    //request 4 bytes
    Wire.requestFrom(address, 4);

    Wire.available();

    int b1 = Wire.read(); // receive a byte
    int b2 = Wire.read(); // receive a byte
    int b3 = Wire.read(); // receive a byte
    int b4 = Wire.read(); // receive a byte
    
    // DEBUG
    #if HYT_DEBUG
        Serial.print("a1: ");
        Serial.println(a1, BIN);
        Serial.print("b1: ");
        Serial.println(b1, BIN);
        Serial.print("a2: ");
        Serial.println(a2, BIN);
        Serial.print("b2: ");
        Serial.println(b2, BIN);
        Serial.print("a3: ");
        Serial.println(a3, BIN);
        Serial.print("b3: ");
        Serial.println(b3, BIN);
        Serial.print("a4: ");
        Serial.println(a4, BIN);
        Serial.print("b4: ");
        Serial.println(b4, BIN);
    #endif
    

    // combine the bits
    rawHum = ( b1 << 8 | b2 ) & 0x3FFF;

    // Mask away 2 last bits see HYT 221 doc
    rawTemp = b3 << 6 | ( unsigned(b4) >> 2 ) ;
    
    return 1;
}

int HYT221::getRawHumidity( void ) {
    return rawHum;
}

int HYT221::getRawTemperature( void ) {
    return rawTemp;
}

double HYT221::getHumidity( void ) {
    //hum = 100.0 / pow( 2, 14 ) * rawHum;
    return (HUM_SCALE * rawHum) / SCALE_MAX;
}

double HYT221::getTemperature( void ) {
    return ( (TEMP_SCALE * rawTemp) / SCALE_MAX ) - TEMP_OFFSET;
}