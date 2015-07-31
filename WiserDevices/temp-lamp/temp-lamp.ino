#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>
#include <DHT.h>

#define GAS_PIN 1
#define DOOR 6
#define DHTTYPE DHT11  
#define DHTPIN 8
#define LAMP 9
#define MQTTPORT 1883
#define MOVE 20

#define H_gas 193492480
#define H_lamp 2090464143
#define H_temp 2090755995
#define H_ar 5863224
#define H_move 2090515612
#define H_door 2090191961


#define ligar(PIN) digitalWrite(PIN,true)
#define desligar(PIN) digitalWrite(PIN,false)

DHT dht(DHTPIN, DHTTYPE);
//variveis
bool lamp = 0,valve = 0,gas_amount,aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 68 };


int t,h,count;
unsigned long int time, lastConnect;


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
        case H_temp:
          t = (int)dht.readTemperature();
          itoa(t,response,10);
          break;  
        case H_ar:
          h = (int)dht.readHumidity();
          itoa(h,response,10);
          break;
        case H_gas:
          gas_amount = analogRead(GAS_PIN);
          gas_amount = map (gas_amount,0,1023,0,100);
          itoa(gas_amount,response,10);
          aux = strlen(response);
          response[aux++] = '%';
          response[aux] = 0;
          break;
        case H_door:
          if (digitalRead(DOOR) == true)
            strcpy(response,"Aberta");
          else
            strcpy(response,"Fechada");
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        case H_lamp:
          if (valor[0] == '1'){ligar(LAMP);Serial.println("ON"); lamp = true;}
          else if (valor[0] == '0'){desligar(LAMP);Serial.println("OFF"); lamp = false;}
          break;
        default:
          return false;
      } 
      //Serial.println(valor);
      break;
  } 
  return true;

}

//VALUE
bool value(uint32_t hash,uint16_t* response,uint16_t valor,uint8_t type){
  Serial.print("Hash :");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_gas:
          *response = analogRead(GAS_PIN);
          *response = map (*response,0,1023,0,100);
          break;
        case H_move:
          *response = count;
          count = 0;
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
//STATE
bool state(uint32_t hash,bool* response,bool valor,uint8_t type){
  
  Serial.print("Hash :");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      Serial.println("akiGET");
      switch(hash){
          Serial.println("akiHASH");
          case H_lamp:
            Serial.println("akiLAMP");
            *response = lamp;
            break;
          case H_gas:
            gas_amount = analogRead(GAS_PIN);
            gas_amount = map (gas_amount,0,1023,0,100);
            Serial.print("Gas amount: ");
            Serial.println(gas_amount);
            if (gas_amount > 55) *response = true;
            else *response = false;
            break;
          case H_door:
            *response = digitalRead(DOOR);
            break;
          default:
            return false;
      }
      break;
    case TATU_SET:
      switch(hash){
          case H_lamp:
            if (valor){ligar(LAMP);Serial.println("ON"); lamp = true;}
            else {desligar(LAMP);Serial.println("OFF"); lamp = false;}
            break;
          default:
            return false;
      } 
      break;  
  }
  return true;
}
Callback callback = {
  .info = info,
  .value = value,
  .state = state
};

// Objetos para exemplo usando interface internet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device("temp-lamp", ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, callback);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
  char aux[16];  
  Serial.begin(9600);
  dht.begin();
  Ethernet.begin(mac, ip);
  pinMode(DHTPIN,INPUT);
  pinMode(LAMP,OUTPUT);
  pinMode(MOVE, INPUT);
  pinMode(DOOR,INPUT);
  
  digitalWrite(MOVE, HIGH);
  attachInterrupt(3, mexeu, FALLING);
   
  Serial.println("Done");
  
  //DEVICECONNECT(client,device);
  Serial.println("Tentando se conectar ao broker");
  while(!client.connect(device.name,"device","boteco@wiser"));
  client.subscribe(device.name,1);
  Serial.println("Conectou ao broker");
  //wifi.getConnectionInfo(connection_info);
  //ipToString(connection_info.ip_address, aux);
  //Mdevice.generateHeader();
  //strcpy(device.ip, aux);
  lastConnect = millis();
  //lamp = digitalRead(RELE);
}
void loop() { client.loop(); 
 time = millis();
 if (time - lastConnect > 600000) {
   Serial.println("reconectando");
   client.disconnect();
   while(!client.connect(device.name,"device","boteco@wiser"));
   client.subscribe(device.name,1);
   lastConnect = millis();
 }
 
}
void mexeu()
{
  count++;
  //iTime = millis();
  Serial.println("mexeu");
}
