#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>
#include <DHT.h>

//Digital pin for the relay
#define LUMINOSITY A3
#define MOVE 3
#define DHTPIN 8
#define SOUND A1
#define GAS A0

#define DHTTYPE 11

//Port to connect with the broker 
#define MQTTPORT 1883

//Hash that represents the attribute "lamp" 
#define H_sound 274653294
#define H_gas 193492480
#define H_temp 2090755995
#define H_ar 5863224
#define H_move 2090515612
#define H_luminosity 1516126306

#define ligar(PIN) digitalWrite(PIN,true)
#define desligar(PIN) digitalWrite(PIN,false)

DHT dht(DHTPIN, DHTTYPE);

//variveis
volatile int soundReading,movement,gas_amount,t,h,luminosity;
int aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 10, 3, 200, 152 };
byte ip[4]    = { 10, 3, 200, 167}; 

unsigned long int time, lastConnect,prevTime,iTime;


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
        Serial.println(soundReading);
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
        luminosity = analogRead(LUMINOSITY);
        switch(code){   
          case TATU_CODE_INFO:
            ITOS(luminosity,response);
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

// Objects to example that uses ethernet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device("device", ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

void setup() {
  char aux[16];  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);  
  
  pinMode(DHTPIN,INPUT);
  pinMode(MOVE, INPUT);
  
  digitalWrite(MOVE, HIGH);
  attachInterrupt(1, mexeu, FALLING);

  //Trying connect to the broker  
  while(!client.connect(device.name,"device","boteco@wiser"));
  client.subscribe(device.name,1);
  Serial.println("Conectado");
}
void loop() { client.loop(); 
  //Watchdog for connection with the broker
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
  movement++;
  Serial.println("mexeu");
}
