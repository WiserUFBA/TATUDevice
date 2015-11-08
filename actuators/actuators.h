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
	    if (*(bool*)RESPONSE){ON(PIN); VAR = true;}\
	    else {OFF(PIN);Serial.println("OFF"); VAR = false;}\
	    break;\
	  default:\
	    return false;\
	} 

#endif