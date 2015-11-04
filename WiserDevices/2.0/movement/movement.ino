#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

//Digital pin for the pir sensor
#define MOVE 3

// Constants to connection with the broker
#define DEVICE_NAME "move"
#define MQTT_USER  "device"
#define MQTT_PASS  "boteco@wiser"
#define MQTTPORT 1883

//Hash that represents the attribute "move" 
#define H_move 2090515612

// Message for annoucement of connection
const char hello[] PROGMEM = DEVICE_NAME " has connected";

//variveis
int aux,movement;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 127}; 


bool get(uint32_t hash,void* response,uint8_t code){
  
  switch(hash){
      case H_move:
        //The I_V_sensor supports INFO and VALUE requests for any integer variable.
        I_V_sensor(movement,response,code);
        movement = 0;
        break;
      default:
        return false;
  }

  
  return true;
  
}

// This is obrigatory, and defines this DEVICE
CREATE_DOD(DEVICE_NAME,
  ADD_LAST_SENSOR("move", "pir", "3"),
  ADD_NONE()
);

// Objects to example that uses ethernet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device(DEVICE_NAME, ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

void setup() {
  //In order to avoid problems caused by external interruptions,
  //is recomended disable the interruption when using the attachInterrupt function;
  cli();//disable interrupts
  
  device.publish_test = &bridge;
  char aux[16];  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  pinMode(MOVE, INPUT);
  digitalWrite(MOVE, HIGH);
  attachInterrupt(1, mexeu, FALLING);
  
  //Trying connect to the broker  
  while(!client.connect(device.name,MQTT_USER,MQTT_PASS));
  client.publish("dev/CONNECTIONS",hello);
  client.subscribe(device.aux_topic_name);
  client.subscribe("dev");
  sei();//unable innterrupts
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

void mexeu(){
  device.interrupt("move","mexeu");
  movement++;
  Serial.println("mexeu");
}
