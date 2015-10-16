#include <stdint.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>

// Constantes do dispositivos
#define H_sound 274653294
#define H_rele 2090684077
#define DEVICE_SAMPLE 0
#define DEVICE_ID 1
#define DEVICE_PAN_ID 1
#define MQTTPORT 1883
#define SOUND_PIN A1
#define RELE 13


// Macro para os atuadores
#define open(PIN) digitalWrite(PIN,true)
#define close(PIN) digitalWrite(PIN,false)


// Constantes da logica do dispositivo
#define THRESHOLD 500
#define SOUND_TIME 2300

// Propriedades do sistema
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 25, 32 };
byte ip[]     = { 192, 168, 25, 40 };

// Variaveis para fazer a l ogica do dispositivo
bool rele;
bool switcher = false;
int soundReading;
unsigned long int time,prevTime;

// Funçao INFO do usuario, para ser usada quando se quer trabalhar com strings  
bool info(uint32_t hash,char* response,char* valor,uint8_t type) {

  switch(type){
    //Retorna o valor de um atributo  
    case TATU_GET:
      switch(hash){
        case H_sound:
          if(switcher) strcpy(response,"alto");
          else strcpy(response,"baixo");
          break;
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
bool value(uint32_t hash,uint16_t* response,uint16_t valor,uint8_t type){
  switch(type){
    //Retorna o valor de um atributo  
    case TATU_GET:
      switch(hash){
        case H_sound:
          *response = soundReading;
          break;
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
bool state(uint32_t hash,bool* response,bool valor,uint8_t type){
switch(type){
    //Retorna o valor de um atributo  
    case TATU_GET:
      switch(hash){
        case H_sound:
          *response = switcher;
          break;
        case H_rele:
          *response = rele;
        default:
          return false;  
      }
      break;
    //Altera o valor de um atributo
    case TATU_SET:
      switch(hash){
        case H_rele:
          if (valor){open(RELE);Serial.println("OPEN"); rele = true;}
          else {close(RELE);Serial.println("CLOSE"); rele = false;}
        default:
          return false;  
      } 
      break;
  } 
  return true;
}

void volume(){
   //Serial.println("Testing...");
   soundReading=analogRead(SOUND_PIN);
   if(soundReading < THRESHOLD){
     prevTime = millis();
     if(!switcher){
      switcher=true;
      Serial.println("alto");
     }
   }
   time = millis();
   //Serial.println(time - prevTime);
   if( (time - prevTime) > SOUND_TIME){
     if(switcher){
       switcher = false;
       Serial.println("baixo");
     }
   }
}
Callback callback{
  .info = info,
  .value = value,
  .state = state
};
// Objetos para exemplo usando interface internet
EthernetClient ethClient;
SETUP("sound", ip, DEVICE_ID, DEVICE_PAN_ID, DEVICE_SAMPLE, server, MQTTPORT, callback, ethClient);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() { Ethernet.begin(mac, ip); 
  Serial.begin(9600);
  pinMode(RELE,OUTPUT);
  prevTime = millis();
  DEVICECONNECT();
}
void loop() { 
  client.loop(); 
  volume();
}
