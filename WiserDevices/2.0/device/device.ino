#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

//Digital pin for the relay
#define LUMINOSITY 0
#define MOVE 3
#define DHTPIN 8
#define SOUND A1


//Port to connect with the broker 
#define MQTTPORT 1883

//Hash that represents the attribute "lamp" 
#define H_lamp 2090464143
#define H_sound 274653294
#define H_gas 193492480
#define H_temp 2090755995
#define H_ar 5863224
#define H_move 2090515612

#define ligar(PIN) digitalWrite(PIN,true)
#define desligar(PIN) digitalWrite(PIN,false)

DHT dht(DHTPIN, DHTTYPE);

//variveis
bool soundReading,movement,gas_amount,t,h,luminosity;
int aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 127}; 

unsigned long int time, lastConnect,prevTime,iTime;


bool get(uint32_t hash,void* response,uint8_t code){
  
  switch(hash){
      case H_move:
        switch(code){   
          case TATU_CODE_INFO:
            itoa(movement,response,10);
            break;
          case TATU_CODE_VALUE:
            *response = movement;
            break;
          default:
            return false;
        } 
        movement = 0;
        break;
      case sound:
        soundReading=analogRead(SOUND_PIN);
        switch(code){
          case TATU_CODE_INFO:
            itoa(soundReading,response,10)
            break;
          case TATU_CODE_VALUE:
            *response = soundReading;
            break;
          default:
            return false;
        }
      case H_temp
        t = (int)dht.readTemperature();
        switch(code){   
          case TATU_CODE_INFO:
            itoa(t,response,10):
            break;
          case TATU_CODE_VALUE:
            *response = t;
            break;
          default:
            return false;
        } 
        break;
      case H_ar
        h = (int)dht.readHumidity();
        switch(code){   
          case TATU_CODE_INFO:
            itoa(h,response,10);
            break;
          case TATU_CODE_VALUE:
            *response = h;
            break;
          default:
            return false;
        } 
        break;
      case H_luminosity
        luminosity = analogRead(LUMINOSITY);
        switch(code){   
          case TATU_CODE_INFO:
            itoa(luminosity,response,10);
            break;
          case TATU_CODE_VALUE:
            *response = luminosity;
            break;
          default:
            return false;
        } 
        break;
      case H_gas:
        gas_amount = analogRead(GAS_PIN);
        gas_amount = map (gas_amount,0,1023,0,100);
        switch(req){   
          case TATU_CODE_INFO:
            itoa(gas_amount,response,10);
            aux = strlen(response);
            response[aux++] = '%';
            response[aux] = 0;
            break;
          case TATU_CODE_VALUE:
            *response = gas_amount;
            break;
          case TATU_CODE_STATE:
            if (gas_amount > 55) *response = true;
            else *response = false;
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
TATUDevice device("rele-pres", ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

void setup() {
  char aux[16];  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);  
  pinMode(LAMP,OUTPUT);
  pinMode(LUMINOSITY,INPUT);
  pinMode(GAS_PIN,INPUT);
  pinMode(DHTPIN,INPUT);
  pinMode(MOVE, INPUT);
  
  digitalWrite(MOVE, HIGH);
  attachInterrupt(1, mexeu, FALLING);

  //Trying connect to the broker  
  while(!client.connect(device.name,"device","boteco@wiser"));
  client.subscribe(device.name,1);

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
