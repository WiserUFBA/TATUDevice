#define DEBUG 1
#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>
#include <Servo.h>

// Definiçao dos hashs
#define H_servo 274290932
#define H_dir 193489476
#define H_esq 193490894

// Definiçoes de constantes importantes
#define SERVO 9
#define MQTTPORT 2883

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
char ap_ssid[] = "wiser2";               
char ap_password[] = "wiser2015";

// Objetos para conexao wifi
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client wifi_client = SFE_CC3000_Client(wifi);

// Variveis
bool lamp = 0,valve = 0;
int pos,new_pos;
byte mac[]    = {  0xFA, 0xCA, 0xAA, 0xE0, 0x31, 0x8C, 0xD3, 0x14 };
byte server[] = { 10, 3, 200, 177 };
byte ip[4]    = { 0, 0, 0, 0}; // Vetor nulo para que nao ocoram erros

Servo myservo;

// Funçao INFO do usuario, para ser usada quando se quer trabalhar com strings  
bool info(uint32_t hash,char* response,char* valor,uint8_t type) {

  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_servo:
          break;
        default:
          return false;  
      }
      break;
    case TATU_SET:
      switch(hash){
        //liga ou desliga uma lampada baseado na primeira letra do valor da requisiçao('T' para ligar e 'F' para desligar)   
        case H_servo:
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
bool value(uint32_t hash,uint16_t* response,uint16_t valor,uint8_t type){
  
  Serial.print("Hash: ");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
          case H_servo:
            *response = pos;
            break;
          default:
            return false;  
      }
      break;
    case TATU_SET:
      switch(hash){
          case H_servo:
            new_pos = valor;
            move();
            break;
          default:
            return false;  
      }
      break;
  } 
  return true;
}
// Funçao STATE do usuario, para ser usada quando se quer trabalhar com booleanos
bool state(uint32_t hash,bool* response,bool valor,uint8_t type){
  
  switch(type){  
    case TATU_GET:
      switch(hash){
          case H_servo:
            break;
          default:
            return false;  
      }
      break;
    case TATU_SET:
      switch(hash){
          case H_servo:
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
  .value = value,
  .state = state
};

void move () {
  if (new_pos > pos)
    for(; pos <= new_pos ; pos += 1){ 
      myservo.write(pos);              
      delay(15); 
    }
  else
    for(; pos >= new_pos; pos -= 1) { 
      myservo.write(pos);              
      delay(15);
    } 
  
}


//callback.info = info;
// Objetos para exemplo usando interface internet
TATUInterpreter interpreter;
TATUDevice device("servo", ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, callback);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , wifi_client);
MQTT_PUBLISH(bridge, client);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
  ConnectionInfo connection_info;
  char aux[16];  
  
  Serial.begin(9600);
  myservo.attach(SERVO);
    
  Serial.println("Tentando conectar ao wifi");
  if ( !wifi.init() || !wifi.connect(ap_ssid, WLAN_SEC_WPA2, ap_password, TIMEOUT_CC3000)) Serial.println("Wifi nao conectado");
  else Serial.println("wifi conectado");
  Serial.println("Done");
    
  Serial.println("Tentando se conectar ao broker");
  if(client.connect(device.name,"device","boteco@wiser")){
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

