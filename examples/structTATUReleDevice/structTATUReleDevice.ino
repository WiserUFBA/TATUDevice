#include <stdint.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

#define LAMP 9
#define VALVE 6
#define MQTTPORT 1883

#define ligar(PIN) digitalWrite(PIN,true)
#define desligar(PIN) digitalWrite(PIN,false)

//internet
#define TIMEOUT_CC3000 30000 // Tempo máximo CC3000 em ms
#define CC3000_INT      2    // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       7    // Pino de ativação
#define CC3000_CS       10   // Pino de seleção, preferivel o pino 10 no UNO
#define IP_ADDR_LEN     4    // Tamanho do IP em bytes

char ap_ssid[] = "Baixo";               
char ap_password[] = "EuEsqueci";
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client wifi_client = SFE_CC3000_Client(wifi);

//variveis
bool lamp = 0,valve = 0;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte server[] = { 192, 168, 0, 103 };
byte ip[4]    = { 192, 168, 0, 68}; // Vetor nulo para que nao ocoram erros
// Funçao do usuario para variaveis do TATU  
bool info(uint32_t hash,char* response,char* valor,uint8_t type) {
  
  Serial.print("Hash :");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_lamp:
          if(lamp) strcpy(response,"ON");
          else strcpy(response,"OFF");
          break;
        case H_valve:
          if(valve) strcpy(response,"OPEN");
          else strcpy(response,"CLOSE");
          break;  
      }
      break;
    case TATU_SET:
      switch(hash){
        case H_lamp:
          if (valor[0] == 'T'){ligar(LAMP);Serial.println("ON"); lamp = true;}
          else if (valor[0] == 'F'){desligar(LAMP);Serial.println("OFF"); lamp = false;}
          break;
        case H_valve:
          if (valor[0] == 'T'){ligar(VALVE);Serial.println("OPEN"); valve = true;}
          else if (valor[0] == 'F'){desligar(VALVE);Serial.println("CLOSE"); valve = false;}
          break;  
      } 
      break;
  } 
  
  Serial.println(valor);
  return false;

}

//VALUE
bool value(uint32_t hash,uint16_t* response,uint16_t* valor,uint8_t type){
  return false;
}
//STATE
bool state(uint32_t hash,bool* response,bool* valor,uint8_t type){
  switch(type){  
    case TATU_GET:
      switch(hash){
          case H_lamp:
            *response = lamp;
            break;
          break;  
      }
      break;
    case TATU_SET:
      switch(hash){
          case H_lamp:
            if (*valor){ligar(LAMP);Serial.println("OPEN"); valve = true;}
            else {desligar(LAMP);Serial.println("CLOSE"); valve = false;}
            break;
          break;  
      } 
  }
  return false;
}

Callback callback = {
  .info = info,
  .value = value,
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
    
  //DEVICECONNECT(client,device);
  Serial.println("Tentando se conectar ao broker");
  if(client.connect(device.name)){
    Serial.println("Conectou ao broker");
    client.subscribe(device.name);
  }
  else Serial.println("Nao conectou ao broker");
  //wifi.getConnectionInfo(connection_info);
  //ipToString(connection_info.ip_address, aux);
  //Mdevice.generateHeader();
  //strcpy(device.ip, aux);
  
  //lamp = digitalRead(RELE);
}
void loop() { client.loop(); 
  
}

