#include <stdint.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

#define H_persiana 1233304888
#define H_gas 193492480
#define H_rain 2090679631
#define H_ultrassonic 3496206460
#define H_luminosity 1516126306

#define OPEN LOW
#define CLOSE HIGH

#define MQTTPORT 1883
#define DIR_1 4
#define DIR_2 7
#define SPEED_PIN_1 5
#define SPEED_PIN_2 6
#define TRIB_PIN 8
#define ULTRASSONIC_PIN 9
#define LUMINOSITY 0
#define GAS_PIN 1
#define RAIN_PIN

#define MOTORSPEED1 122
#define MOTORSPEED2 230
#define MOTORTIME1 1500
#define MOTORTIME2 1500
#define MOTORSTOP 0



//Macro para abrir persiana1
void abrir1(){
  digitalWrite(DIR_1,OPEN);
  analogWrite(SPEED_PIN_1,MOTORSPEED1);
  delay(MOTORTIME1);
  analogWrite(SPEED_PIN_1,MOTORSTOP);
}
//Macro para fechar persiana1
void fechar1(){
  digitalWrite(DIR_1,CLOSE);
  analogWrite(SPEED_PIN_1,MOTORSPEED1);
  delay(MOTORTIME1);
  analogWrite(SPEED_PIN_1,MOTORSTOP);
}
//Macro para abrir persiana2
void abrir2(){
  digitalWrite(DIR_2,OPEN);
  analogWrite(SPEED_PIN_2,MOTORSPEED2);
  delay(MOTORTIME2);
  analogWrite(SPEED_PIN_2,MOTORSTOP);
}
//Macro para fechar persiana2
void fechar2(){
  digitalWrite(DIR_2,CLOSE);
  analogWrite(SPEED_PIN_2,MOTORSPEED2);
  delay(MOTORTIME2);
  analogWrite(SPEED_PIN_2,MOTORSTOP);
}
#define desligar(PIN) digitalWrite(PIN,false);

// Configuraçao do shield CC3000 Sparkfun
#define TIMEOUT_CC3000  30000 // Tempo máximo CC3000 em ms
#define CC3000_INT      2    // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       7    // Pino de ativação
#define CC3000_CS       10   // Pino de seleção, preferivel o pino 10 no UNO
#define IP_ADDR_LEN     4    // Tamanho do IP em bytes

char ap_ssid[] = "wiser";               
char ap_password[] = "wiser2014";
#define MQTTUSER        "device"
#define MQTTPASS        "boteco@wiser"


SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client wifi_client = SFE_CC3000_Client(wifi);

//variveis
int gas_amount;
bool persiana = 0,raining = 0;
byte mac[] = { 0xDE, 0xAD, 0xBC, 0xFA, 0xFE, 0xED };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = {0,0,0,0}; // Vetor nulo para que nao ocoram erros
// Funçao do usuario para variaveis do TATU  

bool callback(uint32_t hash,char* response,char* valor,uint8_t type) {
  
  Serial.print("Hash :");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_persiana:
          if(persiana) strcpy(response,"OPEN");
          else strcpy(response,"CLOSE");
          break;
        case H_luminosity:
          break;
        case H_gas:
          gas_amount = analogRead(GAS_PIN);
          gas_amount = map (gas_amount,0,1023,0,100);
          itoa(gas_amount,response,10);
          break;
        case H_ultrassonic:
          
          break;
        case H_rain:
          if(raining) strcpy(response,"YES");
          else strcpy(response,"NO");
          break;  
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        case H_persiana:
          if (valor[0] == 'T'){abrir1();abrir2();Serial.println("Persian aberta"); persiana = true;}
          else if (valor[0] == 'F'){fechar1();fechar2();Serial.println("Persiana fechada"); persiana = false;}
          break;
        case H_luminosity:
          break;
        case H_gas:
          break;
        case H_ultrassonic:
          break;
        case H_rain:
          break;    
        default:
          return false;
      } 
      break;
  } 
  
  Serial.println(valor);
  return true;

}



// Objetos para exemplo usando interface internet
TATUInterpreter interpreter;
TATUDevice device("DC", ip, 1, 1, 0, server, MQTTPORT, 1, &interpreter, callback);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , wifi_client);
MQTT_PUBLISH(bridge, client);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
  ConnectionInfo connection_info;
  char aux[16];  
  
  Serial.begin(9600);
  pinMode(DIR_1,OUTPUT);
  pinMode(DIR_2,OUTPUT);
  pinMode(LUMINOSITY,INPUT);
  pinMode(GAS_PIN,INPUT);
  pinMode(PRES, INPUT);
  digitalWrite(PRES, HIGH);
  attachInterrupt(0, mexeu, FALLING);
  
  Serial.println("Tentando conectar ao wifi");
  if ( !wifi.init() || !wifi.connect(ap_ssid, WLAN_SEC_WPA2, ap_password, TIMEOUT_CC3000)) Serial.println("Wifi nao conectado");
  else Serial.println("wifi conectado");
  Serial.println("Done");
    
  //DEVICECONNECT(client,device);
  Serial.println("Tentando se conectar ao broker");
  if(client.connect(device.name, MQTTUSER, MQTTPASS)){
    Serial.println("Conectou ao broker");
    client.subscribe(device.name);
  }
  else Serial.println("Nao conectou ao broker");
  //wifi.getConnectionInfo(connection_info);
  //ipToString(connection_info.ip_address, aux);
  //Mdevice.generateHeader();
  //strcpy(device.ip, aux);
  prevTime = millis();
  //lamp = digitalRead(RELE);
}
void loop() { client.loop(); 
  time = millis();
  if ( (time - prevTime) > 6000){
    prevTime = time;
    if (count > 0){ presence = true; Serial.println("alguem");}
    else {presence = false; Serial.println("ninguem");}
    count = 0;
  }
}

void mexeu()
{
  count++;
  iTime = millis();
  Serial.println("mexeu");
}
