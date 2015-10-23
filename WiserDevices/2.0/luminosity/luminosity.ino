#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

//Analogic pin for the photosensor
#define LUMINOSITY 0

//Port to connect with the broker 
#define MQTTPORT 1883

//Hash that represents the attribute "luminosity" 
#define H_luminosity 1516126306

//variveis
int aux,luminosity;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 127}; 

unsigned long int time, lastConnect,prevTime,iTime;

bool get(uint32_t hash,void* response,uint8_t code){
	switch(hash){
			case H_luminosity
				luminosity = analogRead(LUMINOSITY);
				switch(code){   
					case TATU_CODE_INFO;
						itoa(luminosity,response,10);
						break;
					case TATU_CODE_VALUE;
						*response = luminosity;
						break;
					default:
						return false;
				} 
				break;
			default:
				return false;
	} 
	return true;
}

// Objects to example that uses ethernet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device("rele-pres", ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

void setup() {
	char aux[16];  
	Serial.begin(9600);
	Ethernet.begin(mac, ip);  
	pinMode(LUMINOSITY, INPUT);

	//Trying connect to the broker  
	while(!client.connect(device.name,"device","boteco@wiser"));
	client.subscribe(device.name,1);
}
void loop() { client.loop(); 
	//Watchdog for connection with the broker
	time = millis();
	if (time - lastConnect > 600000) {
		Serial.println("reconectando");
		client.disconnect();
		while(!client.connect(device.name,"device","boteco@wiser"));
		client.subscribe(device.name,1);
		lastConnect = millis();
	}
}

