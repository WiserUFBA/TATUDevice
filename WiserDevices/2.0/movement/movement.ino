#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

//Digital pin for the pir sensor
#define MOVE 3

//Port to connect with the broker 
#define MQTTPORT 1883

//Hash that represents the attribute "move" 
#define H_move 2090515612


//variveis
int aux,movement;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 127}; 

//int t,h,count1,count2;
unsigned long int time, lastConnect,prevTime,iTime;


bool get(uint32_t hash,void* response,uint8_t code){
  
  switch(hash){
      case H_move:
        switch(code){   
          case TATU_CODE_INFO;
            itoa(movement,response,10);
            break;
          case TATU_CODE_VALUE;
            *response = movement;
            break;
          default:
            return false;
        } 
        movement = 0;
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
  pinMode(MOVE, INPUT);
  digitalWrite(MOVE, HIGH);
  attachInterrupt(1, mexeu, FALLING);
  
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
void mexeu()
{
  movement++;
  Serial.println("mexeu");
}

