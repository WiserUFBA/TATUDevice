#include "sensors.h"

bool aux_get(uint32_t hash,void* response,uint8_t code){
	switch(hash){

		#ifdef MQ2_SENSOR
	    case H_gas:
	      //The gas_sensor supports INFO,VALUE and STATE requests.
	      gas_sensor(MQ2_SENSOR,gas_amount,response,code);
	      break;
	    #endif

	    #ifdef DHT_SENSOR
	    case H_temp:
          //The dht_temperatures_sensor supports INFO and VALUE requests.
          dht_temperature_sensor(dht,t,response,code);
          break;
        case H_humid:
          //The dht_humidity_sensor supports INFO and VALUE requests.
          dht_humidity_sensor(dht,h,response,code);
          break;
        #endif

		#ifdef LDR_SENSOR
        case H_luminosity:
          //The lumisity_sensor supports INFO and VALUE,requests. 
          luminosity_sensor(LDR_SENSOR,luminosity,response,code);
          break;
	    #endif 

	    default:
	      return false;
	}
	return true; 
}