#include <stdint.h>
#include "Arduino.h"

#ifdef MQ2_SENSOR
#define H_gas 193492480
volatile int gas_amount;
#endif

#ifdef DHT11_SENSOR
#include <DHT.h>
// DHT TYPE
#define DHTTYPE 11
#define H_temp 2090755995
#define H_humid 261814908
DHT dht(DHT11_SENSOR, DHTTYPE);
volatile int h,t;
#endif

#ifdef LDR_SENSOR
#define H_luminosity 1516126306
volatile int luminosity;
#endif

bool aux_get(uint32_t hash,void* response,uint8_t code);
/*bool test_get(uint32_t hash,void* response,uint8_t code){
	switch(hash){

		#ifdef MQ2_SENSOR
	    case H_gas:
	      //The gas_sensor supports INFO,VALUE and STATE requests.
	      gas_sensor(MQ2_SENSOR,gas_amount,response,code);
	      break;
	    #endif

	    default:
	      return false;
	}
	return true; 
}*/
/*void testando(){
	int i;
	for(i = 0; i < 3;i++);
	return;
}*/


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

/*#ifdef
bool get(uint32_t hash,void* response,uint8_t code){
	switch(hash){
		#ifdef DHT_SENSOR
		case H_temp:
	      	//The dht_temperatures_sensor supports INFO and VALUE requests.
	        dht_temperature_sensor(dht,t,response,code);
	        break;
	    #endif
	    case H_humid:
	        //The dht_humidity_sensor supports INFO and VALUE requests.
	        dht_humidity_sensor(dht,h,response,code);
	        break;
	    case H_luminosity:
	        //The lumisity_sensor supports INFO and VALUE,requests. 
	        luminosity_sensor(LUMINOSITY,luminosity,response,code);
	        break;
	    case H_gas:
	        //The gas_sensor supports INFO,VALUE and STATE requests.
	        gas_sensor(GAS,gas_amount,response,code);
	        break;
	    default:
	        return false;
	}
	return true; 
}
#endif*/
/*#define CASO(RESOURCE) 1 \
	#ifdef RESOURCE \
	case HASH: \
	  	//The dht_temperatures_sensor supports INFO and VALUE requests. \
	    dht_temperature_sensor(dht,t,response,code); \
	    break; \
	#endif 


#ifdef algo
#define resources \
	#ifdef DHT_SENSOR \
	case H_temp: \
	  	//The dht_temperatures_sensor supports INFO and VALUE requests. \
	    dht_temperature_sensor(dht,t,response,code); \
	    break; \
	#endif \
	case H_humid: \
	    //The dht_humidity_sensor supports INFO and VALUE requests. \
	    dht_humidity_sensor(dht,h,response,code); \
	    break; \
	case H_luminosity: \
	    //The lumisity_sensor supports INFO and VALUE,requests. \
	    luminosity_sensor(LUMINOSITY,luminosity,response,code); \
	    break; \
	case H_gas: \
	    //The gas_sensor supports INFO,VALUE and STATE requests. \
	    gas_sensor(GAS,gas_amount,response,code); \
	    break; 
#endif*/
