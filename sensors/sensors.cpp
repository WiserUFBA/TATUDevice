#include "sensors.h"

bool aux_get(uint32_t hash,void* response,uint8_t code){
	/*#ifdef GAS
	    Serial.println("GAS");
	#endif*/
	Serial.println(hash);
	switch(hash){
		/*#ifdef GAS
	    case H_gas:
	      	//The gas_sensor supports INFO,VALUE and STATE requests.
	      	gas_sensor(gas_amount,response,code);
	      	break;
	    #endif

	    #ifdef DHT_
	    case H_temp:
          	//The dht_temperatures_sensor supports INFO and VALUE requests.
	        dht_temperature_sensor(dht,t,response,code);
	        break;
        case H_humid:
          	//The dht_humidity_sensor supports INFO and VALUE requests.
          	dht_humidity_sensor(dht,h,response,code);
          	break;
        #endif*/

		#ifdef LDR
       
        case H_luminosity:
        	Serial.println("luminosity");
          	//The lumisity_sensor supports INFO and VALUE,requests. 
          	luminosity_sensor(luminosity,response,code);
          	break;
	    #endif 

	    default:
	    	Serial.println("FALSE");
	     	return false;
	}
	return true; 
}