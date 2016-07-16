#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>
#include "Arduino.h"
#include <TATUDevice.h>

#ifdef LDR_SENSOR
#define LDR 1
volatile int luminosity;
#define H_luminosity 1516126306
int aux;
#endif

/*int aux1;
char aux20[3],aux10[3];*/


//#define v_out(PIN)\
//  analogRead(PIN)*5.0/1024.0

/*#define lux(RESL,PIN)\
    do{\
      ((2500/analogRead(PIN)*5.0/1024.0) - 500)/RESL;\
    }while(false)*/
    /*Irms = emon1.calcIrms(1480);\
    aux1 = Irms*100;\
    VAR = (int)Irms;\
    VAR = VAR*100;\
    aux1 = aux1 - 100;\
    VAR = VAR/100;\
    ITOS(VAR,aux20);\
    ITOS(aux1,aux10);\
    switch(CODE){\
      case TATU_CODE_INFO:\
        strcpy(RESPONSE,aux20);\
        RESPONSE[strlen(aux20)] = '.';\
        RESPONSE[strlen(aux20)+1] = '\0';\
        strcpy(RESPONSE[strlen(RESPONSE)],aux10);\
        aux = strlen((char*)RESPONSE);\
        ((char*)RESPONSE)[aux++] = 'A';\
        ((char*)RESPONSE)[aux] = 0;\*/
            /*aux1 = irms*100;\ 
            aux = (int)irms;\
            aux = aux*100;\
            aux1 = aux1 - 100;\
            aux = aux/100;\
            ITOS(aux,aux20);\
            ITOS(aux1,aux10);\
            switch(CODE){\
              case TATU_CODE_INFO:\
                strcpy((char *)RESPONSE,aux20);\
                RESPONSE[strlen(aux20)] = '.';\
                RESPONSE[strlen(aux20)+1] = '\0';\
                strcpy(RESPONSE[strlen(RESPONSE)],aux10);\
                aux = strlen((char*)RESPONSE);\
                ((char*)RESPONSE)[aux++] = 'A';\
                ((char*)RESPONSE)[aux] = 0;\*/
 #define current_sensor(PIN,VAR,RESPONSE,CODE)\
          do{\
            switch(CODE){\
              case TATU_CODE_INFO:\
                /*strcpy((char *)RESPONSE,"[\"10.3\",\"9.4\"]");*/\
                dtostrf(irms,2,2,(char *)RESPONSE);\
                break;\
              case TATU_CODE_VALUE:\
                ITOI(irms,RESPONSE);\
                break;\
              default:\
                  return false;\
            }\
            irms = 0;\
          }while(false)

#define gas_sensor(PIN,VAR,RESPONSE,CODE)\
      do{\
		    VAR = analogRead(PIN);\
        VAR = map (VAR,0,1023,0,100);\
        switch(CODE){\
          case TATU_CODE_INFO:\
            ITOS(VAR,RESPONSE);\
            aux = strlen((char*)RESPONSE);\
            ((char*)RESPONSE)[aux++] = '%';\
            ((char*)RESPONSE)[aux] = 0;\
            break;\
          case TATU_CODE_VALUE:\
            ITOI(VAR,RESPONSE);\
            break;\
          case TATU_CODE_STATE:\
            if (VAR > 55) BTOB(true,RESPONSE);\
            else BTOB(false,RESPONSE);\
            break;\
        }\
      }while(false)

//VAR = (analogRead(PIN) - 1023) * (-1);\
//VAR = map (VAR,0,1023,0,100);\
//VAR = lux(RESL);\

#define luminosity_sensor(PIN,VAR,RESPONSE,CODE,RESL)\
      do{\
        v_out = ( (float)analogRead(PIN)*(5.0/1024.0));\
        luz = ( ( 2500 / v_out ) - 400)/RESL;\
        /*Serial.print("v_out ");*/\
        /*Serial.println(v_out);*/\
        /*Serial.print("luz ");*/\
        /*Serial.println(luz);\*/\
        VAR = luz;\
        /*Serial.print("var ");*/\
        /*Serial.println(VAR);\*/\
        switch(CODE){\
          case TATU_CODE_INFO:\
            ITOS(VAR,RESPONSE);\
            break;\
          case TATU_CODE_VALUE:\
            ITOI(VAR,RESPONSE);\
            break;\
          default:\
            return false;\
        }\
      }while(false)

#define dht_temperature_sensor(DHT,VAR,RESPONSE,CODE)\
      do{\
        VAR = (int)DHT.readTemperature();\
        switch(CODE){\
          case TATU_CODE_INFO:\
            ITOS(VAR,RESPONSE);\
            break;\
          case TATU_CODE_VALUE:\
            ITOI(VAR,RESPONSE);\
            break;\
          default:\
            return false;\
        }\
      }while(false)
#define dht_humidity_sensor(DHT,VAR,RESPONSE,CODE)\
      do{\
        VAR = (int)DHT.readHumidity();\
        switch(CODE){\
          case TATU_CODE_INFO:\
            ITOS(VAR,RESPONSE);\
            break;\
          case TATU_CODE_VALUE:\
            ITOI(VAR,RESPONSE);\
            break;\
          default:\
            return false;\
        }\
      }while(false)
#define sound_sensor(PIN,VAR,RESPONSE,CODE)\
      do{\
        /*VAR = analogRead(PIN);*/\
        VAR = (20. * log10(analogRead(PIN) / 1.));\
        switch(CODE){\
          case TATU_CODE_INFO:\
            /*ITOS(VAR,RESPONSE);*/\
            dtostrf(VAR,4,3,(char *)RESPONSE);\
            break;\
          case TATU_CODE_VALUE:\
            ITOI(VAR,RESPONSE);\
            break;\
          default:\
            return false;\
        }\
      }while(false)
#define sensor(PIN,VAR,RESPONSE,CODE)\
      do{\
        VAR = analogRead(PIN);\
        switch(CODE){\
          case TATU_CODE_INFO:\
            ITOS(VAR,RESPONSE);\
            break;\
          case TATU_CODE_VALUE:\
            ITOI(VAR,RESPONSE);\
            break;\
          default:\
            return false;\
        }\
      }while(false)
#define att_sensor(VAR,RESPONSE,CODE)\
      do{\
        switch(CODE){\
          case TATU_CODE_INFO:\
            ITOS(VAR,RESPONSE);\
            break;\
          case TATU_CODE_VALUE:\
            ITOI(VAR,RESPONSE);\
            break;\
          default:\
            return false;\
        }\
      }while(false)
#define bool_sensor(VAR,RESPONSE,CODE)\
      do{\
        switch(CODE){\
          case TATU_CODE_INFO:\
            BTOS(VAR,RESPONSE);\
            break;\
          case TATU_CODE_STATE:\
            BTOB(VAR,RESPONSE);\
            break;\
          default:\
            return false;\
        }\
      }while(false)



//bool luminosity_sensor(uint8_t PIN,int VAR,char *RESPONSE,uint8_t CODE);
bool aux_get(uint32_t hash,void* response,uint8_t code);


#endif
