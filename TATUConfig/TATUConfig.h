#ifndef TATUConfig_h
#define TATUConfig_h

#include <stdint.h>

/*
 * All Constants here is created using LRC Hash
 * (Longitudinal Redundancy Check) this HASH
 * is aimed to use with things that don't have many
 * possibilities generally corrections because there are
 * just 255 possibilities since this version of LRC uses only
 * 8 bit (1 byte), you should use an application that asegure
 * the correct names of the strings since a little variation
 * will cause a big difference (not too big but enough) that
 * will save little mistakes that could corrupt the system
 */

/* Configuration Constants */
/* Network */
#define NETWORK_IP 222
#define NETWORK_MAC 166
#define NETWORK_ESSID 255
#define NETWORK_TYPE 53
#define NETWORK_PASS 64
#define NETWORK_ENCRYPTION 108
/* MQTT Broker */
#define MQTT_IP 194
#define MQTT_PORT 22
#define MQTT_USER 28
#define MQTT_PASS 36
/* System Access */
#define ADMIN_USER 249
#define ADMIN_PASSWORD 197
#define ADMIN_SALT 4
/* Device */
#define DEVICE_ID 100
#define DEVICE_PAN 18
#define DEVICE_NAME 208
/* System Configuration */
#define BOARD_PIN 82
#define BOARD_PULLUP 87
#define BOARD_SERIAL 121
#define BOARD_SAMPLE 119
#define BOARD_SLEEP 192
#define BOARD_AREF 27
/* System Details */
#define SYSTEM_VERSION 150
#define SYSTEM_DATE 158

/* Sizes */
#define IP_SIZE 3
#define MAC_SIZE 6
#define ESSID_SIZE 32
#define NAMES_SIZE 20
#define NUMBER_1BYTE 1
#define NUMBER_2BYTE 2
#define NUMBER_4BYTE 4
#define NUMBER_8BYTE 8

enum Configs{
	Network_Ip = 1,
	Network_Mac,
	Network_Essid,
	Network_Type,
	Network_Pass,
	Network_Encryption,
	Mqtt_Ip,
	Mqtt_Port,
	Mqtt_User,
	Mqtt_Pass,
	Admin_User,
	Admin_Password,
	Admin_Salt,
	Device_Id,
	Device_Pan,
	Device_Name,
	Board_Pin,
	Board_Pullup,
	Board_Serial,
	Board_Sample,
	Board_Sleep,
	Board_Aref,
	System_Version,
	System_Date,
};

/* Types */
enum Types{
	Ip_Type,
	Mac_Type,
	ESSID_Type,
	Names_Type,
	Number1B_Type,
	Number2B_Type,
	Number4B_Type,
	Number8B_Type
};

uint8_t LRC_Hash(char *);
int findProperty(uint8_t);
bool editProperty(char *, char *);
bool getProperty(char *);

typedef struct {
	int property;
	int property_size;
	int property_location;
	int property_type;
} Property;

#endif /* TATUConfig_h */
