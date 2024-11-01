/*
  HYT221.cpp - Library for Hygrochip digital humidity sensor
  Created by Marco Benini, on March 21st 2012.
  Released into the public domain.
*/
#ifndef _HYT221_H
#define _HYT221_H

#define HYT_DEBUG 0

class HYT221 {

    private:
        int address;
        int rawTemp;
        int rawHum;
        
    public:
        HYT221(int I2Cadr);
        uint8_t begin( void );
        uint8_t read( void );

        int getRawHumidity( void );
        int getRawTemperature( void ) ;
        double getHumidity( void ) ;
        double getTemperature( void );
    
};

extern HYT221 HYT;


#endif