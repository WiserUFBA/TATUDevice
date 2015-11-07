#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>
#include "Arduino.h"
#include <TATUDevice.h>

#ifdef MQ2_SENSOR
#define MQ2_SENSOR2 MQ2_SENSOR
#define H_gas 193492480
volatile int gas_amount;
#endif

#ifdef DHT11_SENSOR
#include <DHT.h>
// DHT TYPE
#define DHTTYPE 11
#define H_temp 2090755995
#define H_humid 261814908
DHT dht(DHT11_SENSOR_, DHTTYPE);
volatile int h,t;
#endif

#ifdef LDR_SENSOR
#define H_luminosity 1516126306
volatile int luminosity;
int aux;
//har aux[20];
#endif


#define gas_sensor(PIN,VAR,RESPONSE,CODE) \
		VAR = analogRead(PIN); \
        VAR = map (VAR,0,1023,0,100); \
        switch(CODE){ \
          case TATU_CODE_INFO: \
            ITOS(VAR,RESPONSE); \
            aux = strlen((char*)RESPONSE); \
            ((char*)RESPONSE)[aux++] = '%'; \
            ((char*)RESPONSE)[aux] = 0; \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          case TATU_CODE_STATE: \
            if (VAR > 55) BTOB(true,RESPONSE); \
            else BTOB(false,RESPONSE); \
            break; \
        }


#define luminosity_sensor(PIN,VAR,RESPONSE,CODE)  \
        VAR = (analogRead(PIN) - 1023) * (-1); \
        switch(CODE){ \
          case TATU_CODE_INFO: \
            VAR = map (VAR,0,1023,0,100); \
            ITOS(VAR,RESPONSE); \
            aux = strlen((char*)RESPONSE); \
            ((char*)RESPONSE)[aux++] = '%'; \
            ((char*)RESPONSE)[aux] = 0; \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          default: \
            return false; \
        }

#define dht_temperature_sensor(DHT,VAR,RESPONSE,CODE) \
        VAR = (int)DHT.readTemperature(); \
        switch(CODE){ \ 
          case TATU_CODE_INFO: \
            ITOS(VAR,RESPONSE); \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          default: \
            return false; \
        } 
#define dht_humidity_sensor(DHT,VAR,RESPONSE,CODE) \
        VAR = (int)DHT.readHumidity(); \
        switch(CODE){ \ 
          case TATU_CODE_INFO: \
            ITOS(VAR,RESPONSE); \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          default: \
            return false; \
        } 
#define I_V_analog_sensor(PIN,VAR,RESPONSE,CODE) \
        VAR = analogRead(PIN); \
        switch(CODE){ \ 
          case TATU_CODE_INFO: \
            ITOS(VAR,RESPONSE); \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          default: \
            return false; \
        }
#define I_V_sensor(VAR,RESPONSE,CODE) \
        switch(CODE){ \ 
          case TATU_CODE_INFO: \
            ITOS(VAR,RESPONSE); \
            break; \
          case TATU_CODE_VALUE: \
            ITOI(VAR,RESPONSE); \
            break; \
          default: \
            return false; \
        } 

bool aux_get(uint32_t hash,void* response,uint8_t code);
//bool luminosity_sensor(uint8_t PIN,int VAR,char *RESPONSE,uint8_t CODE);

#endif