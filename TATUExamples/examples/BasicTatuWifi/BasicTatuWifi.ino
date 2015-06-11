#include <stdint.h>
#include <SPI.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
#include <PubSubClient.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>

// Configuraçao do shield CC3000 Sparkfun
#define TIMEOUT_CC3000  30000 // Tempo máximo CC3000 em ms
#define CC3000_INT      2    // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       7    // Pino de ativação
#define CC3000_CS       10   // Pino de seleção, preferivel o pino 10 no UNO
#define IP_ADDR_LEN     4    // Tamanho do IP em bytes

// Propriedades do sistema
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 0, 102 };
byte ip[4]    = { 0 }; // Vetor nulo para que nao ocoram erros

// Propriedades de rede
char ap_ssid[] = "";               
char ap_password[] = "";
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client wifi_client = SFE_CC3000_Client(wifi);

// Funçao do usuario para variaveis do TATU
bool callback(uint32_t hash,char* response,char* valor,uint8_t type) {
  // Faça as comparações aqui
  
  //As comparações com o type são feitas para determinar que tipo de ação se espera 
  //TATU_SET serve para alterções no dispositivo TATU_GET para retorna informações requisitadas
  
  // As comparações realizadas com um hash DJB são feitas para se determinar
  // qual o atributo do dispositivo será usado e pode receber quantas 
  // clausulas você quiser 
  // o padrão deve ser return true mas pode ser substiuido por uma resposta true ou false
  
  
  switch(type){  
    case TATU_GET:
      switch(hash){
          break;  
      }
      break;
    case TATU_SET:
      switch(hash){
          break;  
      } 
      //Serial.println(valor);
      break;
  } 
  return true;
}

// Objetos para exemplo usando interface wifi CC3000
TATUInterpreter interpreter;
TATUDevice device("nome", ip, 121, 88, 0, server, 1883, 1, &interpreter, callback);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, 1883, mqtt_callback , wifi_client);
MQTT_PUBLISH(bridge, client);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
  ConnectionInfo connection_info;
  char aux[16];  
  Serial.begin(9600);
  Serial.println("Tentando conectar");
    
  if ( !wifi.init() || !wifi.connect(ap_ssid, WLAN_SEC_WPA2, ap_password, TIMEOUT_CC3000)) while(true);
  Serial.println("Done");

  //DEVICECONNECT(client,device);
  if(client.connect("nome")){
    client.subscribe(device.name);
  }
  else Serial.println("Nao conectou");
  wifi.getConnectionInfo(connection_info);
  ipToString(connection_info.ip_address, aux);
  strcpy(device.ip, aux);
}
void loop() { client.loop(); }
