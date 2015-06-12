/*
 * TATUConfig.cpp
 *
 *  Created on: 12/06/2015
 *      Author: Jeferson
 */
#include "TATUConfig.h"
#include <stdint.h>
#include <string.h>

/* Utilities */
uint8_t LRC_Hash(char *string){
    int i;
    uint8_t LRC = 0;
    for(i = 0; i < strlen(string); i++){
        LRC = ((LRC + string[i]) & 0xFF);
    }
    return (((LRC ^ 0xFF) + 1) & 0xFF);
}

/* Main functions */
Property findProperty(uint8_t lrc_property){
	Property found;

	/* Search and compose */
	switch(lrc_property){
		case NETWORK_IP:
			found.property = Network_Ip;
			found.property_size = IP_SIZE;
			found.property_type = Ip_Type;
			break;
		case NETWORK_MAC:
			found.property = Network_Mac;
			found.property_size = MAC_SIZE;
			found.property_type = Mac_Type;
			break;
		case NETWORK_ESSID:
			found.property = Network_Essid;
			found.property_size = ESSID_SIZE;
			found.property_type = ESSID_Type;
			break;
		case NETWORK_TYPE:
			found.property = Network_Type;
			found.property_size = NUMBER_1BYTE;
			found.property_type = Number1B_Type;
			break;
		case NETWORK_PASS:
			found.property = Network_Pass;
			found.property_size = NAMES_SIZE;
			found.property_type = Mac_Type;
			break;
		case NETWORK_ENCRYPTION:
			found.property = Network_Encryption;
			found.property_size = NUMBER_1BYTE;
			found.property_type = Number1B_Type;
			break;
		case MQTT_IP:
			found.property = Mqtt_Ip;
			found.property_size = IP_SIZE;
			found.property_type = Ip_Type;
			break;
		case MQTT_PORT:
			found.property = Mqtt_Port;
			found.property_size = NUMBER_2BYTE;
			found.property_type = Number2B_Type;
			break;
		case MQTT_USER:
			found.property = Mqtt_User;
			found.property_size = NAMES_SIZE;
			found.property_type = Names_Type;
			break;
		case MQTT_PASS:
			found.property = Mqtt_Pass;
			found.property_size = NAMES_SIZE;
			found.property_type = Names_Type;
			break;
		case ADMIN_USER:
			found.property = Admin_User;
			found.property_size = NAMES_SIZE;
			found.property_type = Names_Type;
			break;
		case ADMIN_PASSWORD:
			found.property = Admin_Password;
			found.property_size = NAMES_SIZE;
			found.property_type = Names_Type;
			break;
		case ADMIN_SALT:
			found.property = Admin_Salt;
			found.property_size = NUMBER_2BYTE;
			found.property_type = Number2B_Type;
			break;
		case DEVICE_ID:
			found.property = Device_Id;
			found.property_size = NAMES_SIZE;
			found.property_type = Names_Type;
			break;
		case DEVICE_PAN:
			found.property = Device_Pan;
			found.property_size = NUMBER_1BYTE;
			found.property_type = Number1B_Type;
			break;
		case DEVICE_NAME:
			found.property = Device_Name;
			found.property_size = NAMES_SIZE;
			found.property_type = Names_Type;
			break;
		case BOARD_PIN:
			found.property = Board_Pin;
			found.property_size = NUMBER_8BYTE;
			found.property_type = Number8B_Type;
			break;
		case BOARD_PULLUP:
			found.property = Board_Pullup;
			found.property_size = NUMBER_4BYTE;
			found.property_type = Number4B_Type;
			break;
		case BOARD_SERIAL:
			found.property = Board_Serial;
			found.property_size = NUMBER_4BYTE;
			found.property_type = Number4B_Type;
			break;
		case BOARD_SAMPLE:
			found.property = Board_Sample;
			found.property_size = NUMBER_2BYTE;
			found.property_type = Number2B_Type;
			break;
		case BOARD_SLEEP:
			found.property = Board_Sleep;
			found.property_size = NUMBER_1BYTE;
			found.property_type = Number1B_Type;
			break;
		case BOARD_AREF:
			found.property = Board_Aref;
			found.property_size = NUMBER_1BYTE;
			found.property_type = Number1B_Type;
			break;
		case SYSTEM_VERSION:
			found.property = System_Version;
			found.property_size = NUMBER_4BYTE;
			found.property_type = Number4B_Type;
			break;
		case SYSTEM_DATE:
			found.property = System_Date;
			found.property_size = NUMBER_8BYTE;
			found.property_type = Number8B_Type;
			break;
		default:
			found.property = 0;
			found.property_size = 0;
			found.property_type = 0;
			found.property_location = 0;
			break;
	}

	/*
	 * If we found a property so make the sum of all sizes
	 * of the previous properties, this sum is the corrected
	 * location of the property we are looking for
	 */
	if(found.property != 0){
		found.property_location = 0;
		switch(lrc_property){
			case SYSTEM_DATE:
				found.property_location += NUMBER_4BYTE;
				/* no break */
			case SYSTEM_VERSION:
				found.property_location += NUMBER_1BYTE;
				/* no break */
			case BOARD_AREF:
				found.property_location += NUMBER_1BYTE;
				/* no break */
			case BOARD_SLEEP:
				found.property_location += NUMBER_2BYTE;
				/* no break */
			case BOARD_SAMPLE:
				found.property_location += NUMBER_4BYTE;
				/* no break */
			case BOARD_SERIAL:
				found.property_location += NUMBER_4BYTE;
				/* no break */
			case BOARD_PULLUP:
				found.property_location += NUMBER_8BYTE;
				/* no break */
			case BOARD_PIN:
				found.property_location += NAMES_SIZE;
				/* no break */
			case DEVICE_NAME:
				found.property_location += NUMBER_1BYTE;
				/* no break */
			case DEVICE_PAN:
				found.property_location += NAMES_SIZE;
				/* no break */
			case DEVICE_ID:
				found.property_location += NUMBER_2BYTE;
				/* no break */
			case ADMIN_SALT:
				found.property_location += NAMES_SIZE;
				/* no break */
			case ADMIN_PASSWORD:
				found.property_location += NAMES_SIZE;
				/* no break */
			case ADMIN_USER:
				found.property_location += NAMES_SIZE;
				/* no break */
			case MQTT_PASS:
				found.property_location += NAMES_SIZE;
				/* no break */
			case MQTT_USER:
				found.property_location += NUMBER_2BYTE;
				/* no break */
			case MQTT_PORT:
				found.property_location += IP_SIZE;
				/* no break */
			case MQTT_IP:
				found.property_location += NUMBER_1BYTE;
				/* no break */
			case NETWORK_ENCRYPTION:
				found.property_location += NAMES_SIZE;
				/* no break */
			case NETWORK_PASS:
				found.property_location += NUMBER_1BYTE;
				/* no break */
			case NETWORK_TYPE:
				found.property_location += ESSID_SIZE;
				/* no break */
			case NETWORK_ESSID:
				found.property_location += MAC_SIZE;
				/* no break */
			case NETWORK_MAC:
				found.property_location += IP_SIZE;
				/* no break */
			case NETWORK_IP:
				found.property_location += 0;
				/* no break */
		}
	}

	return found;
}
