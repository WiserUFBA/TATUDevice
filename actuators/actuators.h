#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <stdint.h>
#include "Arduino.h"
#include <TATUDevice.h>

#define ON(PIN) digitalWrite(PIN,true)
#define OFF(PIN) digitalWrite(PIN,false)

#define bool_actuator(PIN,VAR,RESPONSE,CODE)\
	switch(CODE){\
	  case TATU_CODE_STATE:\
	    if (*(bool*)RESPONSE){ON(PIN); Serial.println("ON"); VAR = true;}\
	    else {OFF(PIN);Serial.println("OFF"); VAR = false;}\
	    break;\
	  default:\
	    return false;\
	} 
#define logic_actuator(PIN,VAR,RESPONSE,CODE)\
	switch(CODE){\
	  case TATU_CODE_STATE:\
	    if (*(bool*)RESPONSE){ON(PIN); Serial.println("ON"); VAR = true;}\
	    else {OFF(PIN);Serial.println("OFF"); VAR = false;}\
	    break;\
	  case TATU_CODE_VALUE:\
	    if (*(int*)RESPONSE == 1){ON(PIN); Serial.println("ON"); VAR = true;}\
	    else if (*(int*)RESPONSE == 0){OFF(PIN);Serial.println("OFF"); VAR = false;}\
	    break;\
	  case TATU_CODE_INFO:\
	    if (*(char*)RESPONSE == '1' || *(char*)RESPONSE == 'T'){ON(PIN); Serial.println("ON"); VAR = true;}\
	    else if (*(char*)RESPONSE == '0' || *(char*)RESPONSE == 'F'){OFF(PIN);Serial.println("OFF"); VAR = false;}\
	    break;\
	  default:\
	    return false;\
	} 

#endif