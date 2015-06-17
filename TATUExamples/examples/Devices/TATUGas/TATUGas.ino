#include <stdint.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

#define H_gas 193492480

// Constantes do dispositivo
#define DEVICE_SAMPLE 0
#define DEVICE_ID 121
#define DEVICE_PAN_ID 88
#define MQTTPORT 1883
#define GAS_PIN A1

//variveis
int aux;
int gas_amount;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte server[] = { 192, 168, 25, 20 };
byte ip[4]    = { 192, 168, 25, 110}; // Vetor nulo para que nao ocoram erros

// Funçao do usuario para variaveis do TATU  
bool info(uint32_t hash,char* response,char* valor,uint8_t type) {  
  Serial.print("Hash :");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_gas:
          gas_amount = analogRead(GAS_PIN);
          gas_amount = map (gas_amount,0,1023,0,100);
          itoa(gas_amount,response,10);
          aux = strlen(response);
          response[aux++] = '%';
          response[aux] = 0;
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        default:
          return false;
      } 
      break;
  } 
  Serial.println(gas_amount);
  return true;
}
bool value(uint32_t hash,uint16_t* response,uint16_t valor,uint8_t type) {
  
  Serial.print("Hash :");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_gas:
          *response = analogRead(GAS_PIN);
          *response = map (*response,0,1023,0,100);
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        default:
          return false;
      } 
      break;
  } 
  Serial.println(gas_amount);
  return true;
}
bool state(uint32_t hash,bool* response,bool valor,uint8_t type) {
  Serial.print("Hash :");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_gas:
          gas_amount = analogRead(GAS_PIN);
          gas_amount = map (gas_amount,0,1023,0,100);
          Serial.print("Gas amount: ");
          Serial.println(gas_amount);
          if (gas_amount > 55) *response = true;
          else *response = false;
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        default:
          return false;
      } 
      break;
  } 
  Serial.println(gas_amount);
  return true;
}

Callback callback= {
  .info = info,
  .value = value,
  .state = state
};


// Objetos para exemplo usando interface internet
EthernetClient ethClient;
SETUP("gas", ip, DEVICE_ID, DEVICE_PAN_ID, DEVICE_SAMPLE, server, MQTTPORT, callback, ethClient);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
  
  Ethernet.begin(mac,ip);
  Serial.begin(9600);
  pinMode(GAS_PIN,INPUT);
    
  DEVICECONNECT();
}

void loop() { client.loop(); }

