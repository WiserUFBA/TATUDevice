#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

//Analogic pin for the gas sensor
#define GAS_PIN A0

//Port to connect with the broker 
#define MQTTPORT 1883

//Hash that represents the attribute "gas" 
#define H_gas 193492480

#define ligar(PIN) digitalWrite(PIN,true)
#define desligar(PIN) digitalWrite(PIN,false)

//variveis
int gas_amount,aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 127}; 

//int t,h,count1,count2;
unsigned long int time, lastConnect,prevTime,iTime;


bool get(uint32_t hash,void* response,uint8_t req){
  
  switch(hash){
      case H_gas:
        gas_amount = analogRead(GAS_PIN);
        gas_amount = map (gas_amount,0,1023,0,100);
        switch(req){   
          case TATU_CODE_INFO;
            itoa(gas_amount,response,10);
            aux = strlen(response);
            response[aux++] = '%';
            response[aux] = 0;
            break;
          case TATU_CODE_VALUE;
            *response = gas_amount;
            break;
          case TATU_CODE_STATE;
            if (gas_amount > 55) *response = true;
            else *response = false;
            break;
        } 
        break;
      default:
        return false;
  }

  
  return true;
  
}
// Objetos para exemplo usando interface internet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device("rele-pres", ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
  char aux[16];  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);  
 
  Serial.println("Done");
  
  //Trying connect to the broker  
  while(!client.connect(device.name,"device","boteco@wiser"));
  client.subscribe(device.name,1);

}
void loop() { client.loop(); 
  time = millis();
  if (time - lastConnect > 600000) {
    Serial.println("reconectando");
    client.disconnect();
    while(!client.connect(device.name,"device","boteco@wiser"));
    client.subscribe(device.name,1);
    lastConnect = millis();
  }

 
}

