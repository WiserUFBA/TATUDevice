#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <sensors.h>
#include <actuators.h>
#include <string.h>
#include <DHT.h>

// Pins used
#define GAS A1
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
#define H_humid 261814908
#define H_move 2090515612
#define H_door 2090191961

// Message for annoucement of connection
const char hello[] PROGMEM = DEVICE_NAME " has connected";

#define ON(PIN) digitalWrite(PIN,true)
#define OFF(PIN) digitalWrite(PIN,false)

DHT dht(DHTPIN, DHTTYPE);

//variveis
bool lamp = 0,aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 68 };

//int t,h,count;
volatile int soundReading,movement,gas_amount,t,h;

  bool get(uint32_t hash,void* response,uint8_t code){
    switch(hash){
        case H_move:
          //The I_V_sensor supports INFO and VALUE requests for any integer variable.
          att_sensor(movement,response,code);
          movement = 0;
          break;
        case H_temp:
          //The dht_temperatures_sensor supports INFO and VALUE requests.
          dht_temperature_sensor(dht,t,response,code);
          break;
        case H_humid:
          //The dht_humidity_sensor supports INFO and VALUE requests.
          dht_humidity_sensor(dht,h,response,code);
          break;
        case H_gas:
          //The gas_sensor supports INFO,VALUE and STATE requests.
          gas_sensor(GAS,gas_amount,response,code);
          break;
        case H_lamp:
          bool_sensor(lamp,response,code);
        case H_door:
          *(bool*)response = digitalRead(DOOR);
        default:
          return false;
    }
    return true; 
  }
bool set(uint32_t hash,uint8_t code,void* response){
  switch(hash){
    case H_lamp:
      bool_actuator(LAMP,lamp,response,code);
      break;
    default:
      return false;
    }
    return true; 
}


// Objetos para exemplo usando interface internet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device(DEVICE_NAME, ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get,set);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

// This is obrigatory, and defines this DEVICE
CREATE_DOD(DEVICE_NAME,
  ADD_SENSORS("lamp", "relay", "9")
  ADD_SENSORS("move", "pir", "20")
  ADD_SENSORS("gas", "mq2", "A1")
  ADD_SENSORS("door", "mq2", "6")
  ADD_SENSORS("temp", "dht11", "8")
  ADD_LAST_SENSOR("humid", "dht11", "8"),
  ADD_NONE()
);


//SETUP
void setup() {
  //In order to avoid problems caused by external interruptions,
  //is recomended disable the interruption when using the attachInterrupt function;
  Serial.println("Trying connect to the broker");  
  cli();//disable interruptions

  device.pub= &bridge;

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
   
  //Trying connect to the broker
  //Serial.println("Trying connect to the broker");  
  while(!client.connect(device.name,MQTT_USER,MQTT_PASS));
  client.publish("dev/CONNECTIONS",hello);
  client.subscribe(device.aux_topic_name);
  client.subscribe("dev");
  sei();//unable interruptions
  Serial.println("Conected!!");
}

//LOOP
void loop() { 
  client.loop(); 
  //Watchdog for connection with the broker
  if (!client.connected()) {
    reconnect();
  }
  
}


//PERSONAL FUNCTIONS
void mexeu(){
  device.interrupt("move","mexeu");
  movement++;
  Serial.println("mexeu");
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connect(device.name, MQTT_USER, MQTT_PASS)) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.publish("dev/CONNECTIONS",hello)) {
      Serial.println("connected");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

