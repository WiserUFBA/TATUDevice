#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

//Digital pin for the relay
#define LAMP 8

//Port to connect with the broker 
#define MQTTPORT 1883

//Hash that represents the attribute "lamp" 
#define H_lamp 2090464143

#define ligar(PIN) digitalWrite(PIN,true)
#define desligar(PIN) digitalWrite(PIN,false)

//variveis
bool lamp;
int aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 127}; 

//int t,h,count1,count2;
unsigned long int time, lastConnect,prevTime,iTime;


bool get(uint32_t hash,void* response,uint8_t code){
  
  switch(hash){
      case H_lamp
        switch(code){   
          case TATU_CODE_INFO;
            if(lamp) strcpy(response,"ON");
            else strcpy(response,"OFF");
            break;
          case TATU_CODE_STATE;
            *response = lamp;
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
bool set(uint32_t hash,void* req,uin8_t code){
    switch(hash){
      case H_lamp:
        switch(code){   
          case TATU_CODE_INFO;
            if (req[0] == '1'){ligar(LAMP);Serial.println("ON"); lamp = true;}
            else if (req[0] == '0'){desligar(LAMP);Serial.println("OFF"); lamp = false;}
            break;
          case TATU_CODE_STATE;
            if (req){ligar(LAMP);Serial.println("ON"); lamp = true;}
            else {desligar(LAMP);Serial.println("OFF"); lamp = false;}
            break;
          default:
            return false;
        } 
        break;
      default:
        return false;
  }

}
// Objects to example that uses ethernet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device("rele-pres", ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get,set);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

void setup() {
  char aux[16];  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);  
  pinMode(LAMP,OUTPUT);   

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

