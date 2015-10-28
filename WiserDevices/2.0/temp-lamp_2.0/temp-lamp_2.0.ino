#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>
#include <DHT.h>

// Pins used
#define GAS_PIN 1
#define DOOR 6
#define DHTPIN 8
#define LAMP 9
#define MOVE 20

// DHT TYPE
#define DHTTYPE DHT11  

// Constants to connection with the broker
#define DEVICE_NAME "temp-lamp"
#define MQTT_USER  "device"
#define MQTT_PASS  "boteco@wiser"
#define MQTTPORT 1883

//Hash that represents the attributes
#define H_gas 193492480
#define H_lamp 2090464143
#define H_temp 2090755995
#define H_ar 5863224
#define H_move 2090515612
#define H_door 2090191961

// Message for annoucement of connection
const char hello[] PROGMEM = DEVICE_NAME " has connected";

#define ON(PIN) digitalWrite(PIN,true)
#define OFF(PIN) digitalWrite(PIN,false)

DHT dht(DHTPIN, DHTTYPE);

//variveis
bool lamp = 0,gas_amount,aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 68 };

int t,h,count;

bool get(uint32_t hash,void* response,uint8_t code){
  switch(hash){
      case H_move:
        switch(code){   
          case TATU_CODE_INFO:
            ITOS(movement,response);
            break;
          case TATU_CODE_VALUE:
            ITOI(movement,response);
            break;
          default:
            return false;
        } 
        movement = 0;
        break;
      case H_sound:
        soundReading = analogRead(SOUND);
        switch(code){
          case TATU_CODE_INFO:
            ITOS(soundReading,response);
            break;
          case TATU_CODE_VALUE:
            ITOI(soundReading,response);
            break;
          default:
            return false;
        }
        break;
      case H_temp:
        t = (int)dht.readTemperature();
        switch(code){   
          case TATU_CODE_INFO:
            ITOS(t,response);
            break;
          case TATU_CODE_VALUE:
            ITOI(t,response);
            break;
          default:
            return false;
        } 
        break;
      case H_ar:
        h = (int)dht.readHumidity();
        switch(code){   
          case TATU_CODE_INFO:
            ITOS(h,response);
            break;
          case TATU_CODE_VALUE:
            ITOI(h,response);
            break;
          default:
            return false;
        } 
        break;
      case H_luminosity:
        luminosity = (analogRead(LUMINOSITY) - 1023) * (-1);
        
        switch(code){   
          case TATU_CODE_INFO:
            luminosity = map (luminosity,0,1023,0,100);
            ITOS(luminosity,response);
            aux = strlen((char*)response);
            ((char*)response)[aux++] = '%';
            ((char*)response)[aux] = 0;
            break;
          case TATU_CODE_VALUE:
            ITOI(luminosity,response);
            break;
          default:
            return false;
        } 
        break;
      case H_gas:
        gas_amount = analogRead(GAS);
        gas_amount = map (gas_amount,0,1023,0,100);
        switch(code){   
          case TATU_CODE_INFO:
            ITOS(gas_amount,response);
            aux = strlen((char*)response);
            ((char*)response)[aux++] = '%';
            ((char*)response)[aux] = 0;
            break;
          case TATU_CODE_VALUE:
            ITOI(gas_amount,response);
            break;
          case TATU_CODE_STATE:
            if (gas_amount > 55) BTOB(true,response);
            else BTOB(false,response);
            break;
        } 
        break;
      default:
        return false;
  }
  return true; 
}

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
            if (valor){ON(LAMP);Serial.println("ON"); lamp = true;}
            else {OFF(LAMP);Serial.println("OFF"); lamp = false;}
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
TATUDevice device(DEVICE_NAME, ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, callback);
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
