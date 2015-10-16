#include <stdint.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>

// Constantes do dispositivos
#define DEVICE_SAMPLE 0
#define DEVICE_ID 121
#define DEVICE_PAN_ID 88
#define MQTTPORT 1883

// Propriedades do sistema
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 25, 20 };
byte ip[]     = { 192, 168, 25, 40 };

// Funçao INFO do usuario, para ser usada quando se quer trabalhar com strings  
bool callback(uint32_t hash,char* response,char* valor,uint8_t type) {
  /*As comparações com o type são feitas para determinar que tipo de ação se espera 
  TATU_SET serve para alterções no dispositivo TATU_GET para retorna informações requisitadas*/
  
  /* As comparações realizadas com um hash DJB são feitas para se determinar
  qual o atributo do dispositivo será usado e pode receber quantas 
  clausulas você quiser 
  o padrão deve ser return true mas deve ser substituido por uma resposta false se nao for possivel executar a requisicao*/
  
  switch(type){
    //Retorna o valor de um atributo  
    case TATU_GET:
      switch(hash){
          default:
            return false;  
      }
      break;
    //Altera o valor de um atributo
    case TATU_SET:
      switch(hash){
          default:
            return false;  
      } 
      break;
  } 
  return true;
}

/* Tambem podem ser usados essas duas formas de callback, alem do uso de uma estrutura que e demonstrada no exemplo structTATUReleDevice
Funçao VALUE do usuario, para ser usada quando se quer trabalhar com inteiros
bool callback(uint32_t hash,uint16_t* response,uint16_t valor,uint8_t type);
Funçao STATE do usuario, para ser usada quando se quer trabalhar com booleanos
bool callback(uint32_t hash,bool* response,bool valor,uint8_t type);*/

// Objetos para exemplo usando interface internet
EthernetClient ethClient;
SETUP("name", ip, DEVICE_ID, DEVICE_PAN_ID, DEVICE_SAMPLE, server, MQTTPORT, callback, ethClient);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() { Ethernet.begin(mac, ip); 
  Serial.begin(9600);
 
  DEVICECONNECT();
}
void loop() { client.loop(); }
