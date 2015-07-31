#include <SPI.h>
#include <stdint.h>
#include <IRremote.h>
#include <TATUInterpreter.h>
#include <TATUDevice.h>
#include <PubSubClient.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>

#define H_value 277698370
#define H_status 478842185
#define H_air 193486209

//internet
#define TIMEOUT_CC3000 30000 // Tempo máximo CC3000 em ms
#define CC3000_INT      2    // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       7    // Pino de ativação
#define CC3000_CS       10   // Pino de seleção, preferivel o pino 10 no UNO
#define IP_ADDR_LEN     4    // Tamanho do IP em bytes

char ap_ssid[] = "wiser2";               
char ap_password[] = "wiser2015";
byte mac[]    = {  0xFA, 0xCA, 0xAA, 0x88, 0x3B, 0x2D, 0x9A, 0x6 };
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client wifiClient = SFE_CC3000_Client(wifi);

Callback callback = {
  .info = info,
  .value = value,
  .state = status
};

#define WLAN_SECURITY   WLAN_SEC_WPA2
#define MQTTPORT        2883
#define MQTTUSER        "device"
#define MQTTPASS        "boteco@wiser"

// Constantes de usuario
#define H_state 0x10614A06

// Atributos de sistema
byte server[] = {10, 3, 200, 177};
byte null_ip[] = {0,0,0,0};

// Objetos de comunicação

IRsend arc;

// Objetos de sistema
TATUInterpreter interpreter;
TATUDevice device("air", null_ip, 1, 2, 0, server, MQTTPORT, 1, &interpreter, callback);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , wifiClient);
MQTT_PUBLISH(bridge, client);

// Variaveis de ambiente
int temp = 16;
bool state = false;
unsigned long int time, lastConnect;


void setup(void)
{
    Serial.begin(9600);
    
    communication_begin();
    broker_begin();
    lastConnect = millis();

}

void loop(void){ client.loop();
 time = millis();
 if (time - lastConnect > 600000) {
   Serial.println("reconectando");
   client.disconnect();
   while(!client.connect(device.name,MQTTUSER,MQTTPASS));
   client.subscribe(device.name,1);
   lastConnect = millis();
 }
}

/* Initialise the module */
void communication_begin(){
    Serial.println("Tentando conectar ao wifi");
  if ( !wifi.init() || !wifi.connect(ap_ssid, WLAN_SECURITY, ap_password, TIMEOUT_CC3000)) Serial.println("Wifi nao conectado");
  else Serial.println("wifi conectado");
  Serial.println("Done");
}

/* Connect with the determinated broker */
void broker_begin(){
    Serial.println("Connecting with broker");
    while(!client.connect(device.name, MQTTUSER, MQTTPASS));
    Serial.println("Conected with broker");
    client.subscribe(device.name);
}


// Driver para o Ar Condicionado
bool info(uint32_t hash,char* response,char* valor,uint8_t type) {
    //Serial.println("Hash:");
    //Serial.println(hash);
    //Serial.println("Callback");
    switch(type){
        case TATU_SET:
            //erial.println("Aki1");
            switch(hash){
                case H_value:
                    temp = atoi(valor);
                    //Serial.println("Aki2");
                    if(temp == 0){ arc.desligar(); state = false; temp = 16;Serial.println("OFF");}
                    else{
                        if(temp < 16)
                            temp = 16;
                        else if(temp > 31)
                            temp = 31;
                        arc.sendTemp(temp);
                        state = true;
                        //Serial.print("Enviou: ");
                        //Serial.println(temp);
                    }
                    break;
                case H_status:
                    if(valor[0] == 'T'){arc.sendTemp(temp);state = true;Serial.println("ON");}
                    else if (valor[0] == 'F'){arc.desligar();state = false;Serial.println("OFF");} 
                    break;
                default:
                    return false;
            }
            break;
        case TATU_GET:
            switch(hash){
                case H_value:
                    itoa(temp, response, 10);
                    break;
                case H_status:
                  if(state) response[0] = 'T';         
                  else response[0] = 'F';  
                  response[1] = 0;
                  break;
                default:
                  return false;
            }
            break;
    }
    //Serial.println(temp);
    //Serial.println("Fim Callback");
    return true;
}
bool value(uint32_t hash,uint16_t* response,uint16_t valor,uint8_t type){
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_air:
          *response = temp;
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        case H_air:
          temp = valor;
          //Serial.println("Aki2");
          if(temp == 0){ arc.desligar(); state = false; temp = 16;Serial.println("OFF");}
          else{
              if(temp < 16)
                  temp = 16;
              else if(temp > 31)
                  temp = 31;
              arc.sendTemp(temp);
              state = true;
              //Serial.print("Enviou: ");
              //Serial.println(temp);
          }
          break;
        default:
          return false;
      } 
      break;
  } 
  
  return true;
}
bool status(uint32_t hash,bool* response,bool valor,uint8_t type){
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_air:
          *response = state;
          break;

        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        case H_air:
          if(valor){arc.sendTemp(temp);state = true;Serial.println("ON");}
          else {arc.desligar();state = false;Serial.println("OFF");} 
          break;
        default:
          return false;
      } 
      break;
  } 
  
  return true;
}

