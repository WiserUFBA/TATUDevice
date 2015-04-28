#include <stdint.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>

// Propriedades do sistema
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 0, 100 };
byte ip[]     = { 192, 168, 0, 120 };

// Funçao do usuario para variaveis do TATU
bool callback(uint32_t hash,char* response) {
  /* Aqui fica o cdigo do usuario a ser executado quando houver requisiçes */
  // Faça as comparações aqui
  // Todas comparações são realizadas com um hash DJB o switch abaixo
  // pode receber quantas clausulas você quiser o padrão deve ser
  // return true mas pode ser substiuido por uma resposta true ou false
  switch(hash){
    default: return true;
  }
}

// Objetos para exemplo usando interface internet
EthernetClient ethClient;
TATUInterpreter interpreter;
TATUDevice device("nome", ip, 121, 88, 0, server, 1883, 1, &interpreter, callback);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, 1883, mqtt_callback , ethClient);
MQTT_PUBLISH(bridge, client);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() { Ethernet.begin(mac, ip); DEVICECONNECT(client,device); }
void loop() { client.loop(); }
