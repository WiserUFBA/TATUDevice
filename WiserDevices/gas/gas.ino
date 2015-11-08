#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <sensors.h>
#include <string.h>

//Analogic pin for the gas sensor
#define GAS A0

// Constants to connection with the broker
#define DEVICE_NAME "gas"
#define MQTT_USER  "device"
#define MQTT_PASS  "boteco@wiser"
#define MQTTPORT 1883

//Hash that represents the attribute "gas" 
#define H_gas 193492480

// Message for annoucement of connection
const char hello[] PROGMEM = DEVICE_NAME " has connected";

//variveis
int gas_amount,aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 127}; 

//int t,h,count1,count2;
unsigned long int time, lastConnect,prevTime,iTime;


bool get(uint32_t hash,void* response,uint8_t code){
  
  switch(hash){
      case H_gas:
        //The gas_sensor supports INFO,VALUE and STATE requests.
        gas_sensor(GAS,gas_amount,response,code);
        break;
      default:
        return false;
  }

  
  return true;
  
}

// This is obrigatory, and defines this DEVICE
CREATE_DOD(DEVICE_NAME,
  ADD_LAST_SENSOR("gas", "mq2", "A0"),
  ADD_NONE()
);

// Objetos para exemplo usando interface internet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device(DEVICE_NAME, ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
  char aux[16];  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);  
 
  
  //Trying connect to the broker  
  while(!client.connect(device.name,MQTT_USER,MQTT_PASS));
  client.publish("dev/CONNECTIONS",hello);
  client.subscribe(device.aux_topic_name);
  client.subscribe("dev");
  
  Serial.println("Conected");

}
void loop() { client.loop(); 
  //Watchdog for connection with the broker
  if (!client.connected()) {
    reconnect();
  }

 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connect(device.name, MQTT_USER, MQTT_PASS)) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.publish("dev",device.name)) {
      Serial.println("connected");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
