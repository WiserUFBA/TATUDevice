#define DEBUG 1
#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

// Definiçao dos hashs
#define H_lamp 2090464143
#define H_valve 277698403

// Definiçoes de constantes importantes
#define LAMP 9
#define VALVE 6
#define MQTTPORT 1883

// Macro para os atuadores
#define turn_on(PIN) digitalWrite(PIN,true)
#define turn_off(PIN) digitalWrite(PIN,false)
#define open_valve(PIN) digitalWrite(PIN,true)
#define close_valve(PIN) digitalWrite(PIN,false)

// Internet
#define TIMEOUT_CC3000 30000 // Tempo máximo CC3000 em ms
#define CC3000_INT      2    // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       7    // Pino de ativação
#define CC3000_CS       10   // Pino de seleção, preferivel o pino 10 no UNO
#define IP_ADDR_LEN     4    // Tamanho do IP em bytes

// Variaveis de conexao
char ap_ssid[] = "";               
char ap_password[] = "";

// Objetos para conexao wifi
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client wifi_client = SFE_CC3000_Client(wifi);

// Variveis
bool lamp = 0,valve = 0;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte server[] = { 192, 168, 25, 20 };
byte ip[4]    = { 0, 0, 0, 0}; // Vetor nulo para que nao ocoram erros

// Funçao INFO do usuario, para ser usada quando se quer trabalhar com strings  
bool info(uint32_t hash,char* response,char* valor,uint8_t type) {
  
  Serial.print("Hash: ");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
        //Devolve uma resposta em string dizendo se a lampada esta ligada ou nao 
        case H_lamp:
          if(lamp) strcpy(response,"ON");
          else strcpy(response,"OFF");
          break;
        //Devolve uma resposta em string dizendo se a valvula esta aberta ou nao
        case H_valve:
          if(valve) strcpy(response,"OPEN");
          else strcpy(response,"CLOSE");
          break;
        default:
          return false;  
      }
      break;
    case TATU_SET:
      switch(hash){
        //liga ou desliga uma lampada baseado na primeira letra do valor da requisiçao('T' para ligar e 'F' para desligar)   
        case H_lamp:
          if (valor[0] == 'T'){turn_on(LAMP);Serial.println("ON"); lamp = true;}
          else if (valor[0] == 'F'){turn_off(LAMP);Serial.println("OFF"); lamp = false;}
          break;
        //abre ou fecha uma valvula baseado na primeira letra do valor da requisiçao('T' para abrir e 'F' para fechar)
        case H_valve:
          if (valor[0] == 'T'){close_valve(VALVE);Serial.println("OPEN"); valve = true;}
          else if (valor[0] == 'F'){open_valve(VALVE);Serial.println("CLOSE"); valve = false;}
          break;
        default:
          return false;  
      } 
      break;
  } 
  
  Serial.println(valor);
  return true;

}

// Funçao VALUE do usuario, para ser usada quando se quer trabalhar com inteiros
// se quiser usar essa funçao basta descomentar 
/*bool value(uint32_t hash,uint16_t* response,uint16_t valor,uint8_t type){
  
  Serial.print("Hash: ");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
          case H_lamp:
            *response = lamp;
            break;
          case H_valve:
            *response = valve;
            break;
          default:
            return false;  
      }
      break;
    case TATU_SET:
      switch(hash){
          case H_lamp:
            if (valor){turn_on(LAMP);Serial.println("ON"); valve = true;}
            else {turn_off(LAMP);Serial.println("OFF"); valve = false;}
            break;
          case H_valve:
            if (valor){close_valve(VALVE);Serial.println("OPEN"); valve = true;}
            else {open_valve(VALVE);Serial.println("CLOSE"); valve = false;}
            break;
          default:
            return false;  
      }
      break;
  } 
}*/
// Funçao STATE do usuario, para ser usada quando se quer trabalhar com booleanos
bool state(uint32_t hash,bool* response,bool valor,uint8_t type){
  
  Serial.print("Hash: ");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
          //Devolve uma resposta booleana dizendo se a lampada esta ligada ou nao
          case H_lamp:
            *response = lamp;
            break;
          //Devolve uma resposta booleana dizendo se a valvula esta aberta ou nao
          case H_valve:
            *response = valve;
            break;
          default:
            return false;  
      }
      break;
    case TATU_SET:
      switch(hash){
          //liga ou desliga uma lampada baseado no valor booleano de uma requisiçao(true para ligar e false para desligar)
          case H_lamp:
            if (valor){turn_on(LAMP);Serial.println("ON"); valve = true;}
            else {turn_off(LAMP);Serial.println("OFF"); valve = false;}
            break;
          //abre ou fecha uma lampada baseado no valor booleano de uma requisiçao(true para abrir e false para fechar)
          case H_valve:
            if (valor){close_valve(VALVE);Serial.println("OPEN"); valve = true;}
            else {open_valve(VALVE);Serial.println("CLOSE"); valve = false;}
            break;
          default:
            return false;  
      }
      break; 
  }
  return true;
}

//para se utilizar de mais de uma funçao do TATUDevice, se nao quiser usar uma delas basta atribuir um NULL
Callback callback = {
  .info = info,
  .value = NULL,
  .state = state
};

//callback.info = info;
// Objetos para exemplo usando interface internet
TATUInterpreter interpreter;
TATUDevice device("rele", ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, callback);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , wifi_client);
MQTT_PUBLISH(bridge, client);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
  ConnectionInfo connection_info;
  char aux[16];  
  
  Serial.begin(9600);
  pinMode(LAMP,OUTPUT);
  pinMode(VALVE,OUTPUT);

  Serial.println("Tentando conectar ao wifi");
  if ( !wifi.init() || !wifi.connect(ap_ssid, WLAN_SEC_WPA2, ap_password, TIMEOUT_CC3000)) Serial.println("Wifi nao conectado");
  else Serial.println("wifi conectado");
  Serial.println("Done");
    
  Serial.println("Tentando se conectar ao broker");
  if(client.connect(device.name)){
    Serial.println("Conectou ao broker");
    client.subscribe(device.name);
  }
  else Serial.println("Nao conectou ao broker");
  
  //Passa o ip conseguido para o device
  wifi.getConnectionInfo(connection_info);
  ipToString(connection_info.ip_address, aux);
  device.generateHeader();
  strcpy(device.ip, aux);
  
}
void loop() { client.loop(); 
  
}

