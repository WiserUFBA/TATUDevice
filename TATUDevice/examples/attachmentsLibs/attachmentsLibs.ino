#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>
#include <DHT.h>

/*
Attachments are how refer to external devices(sensors and actuators) that are 
connected to the arduino node. 
*/
/* These are the libraries containing attachments
  
They are files that can be use in order to minimize the programming time
through the use of macros that implements the use of attachments that already
was used before

In our repository there is that two libraries containing some code that we already has implemented
In those libraries you can found simple codes for use with DHT11 sensor, lamp(relay or any sort of on/off device), mq2 sensor, and others. 
*/
#include <sensors.h>
#include <actuators.h>

// Pins used
#define GAS 1
#define DOOR 6
#define DHTPIN 8
#define LAMP 9
#define MOVE 20

// DHT TYPE
#define DHTTYPE DHT11  

// Constants to connection with the broker
#define DEVICE_NAME "device"
#define MQTT_USER  "user"
#define MQTT_PASS  "password"
#define MQTTPORT 1883

//Hash that represents the attributes
#define H_gas 193492480
#define H_lamp 2090464143
#define H_temp 2090755995
#define H_humid 261814908
#define H_move 2090515612

// Message for annoucement of connection
const char hello[] PROGMEM = DEVICE_NAME " has connected";

DHT dht(DHTPIN, DHTTYPE);

//variveis
bool lamp = 0,aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 1, 14 };
byte ip[4]    = { 192, 168, 1, 27 };

//int t,h,count;
volatile int soundReading,movement,gas_amount,t,h;

  bool get(uint32_t hash,void* response,uint8_t code){
    switch(hash){
        case H_move:
          //The att_sensor supports INFO and VALUE requests for any integer variable. he is used when you dont need describe the attachment's pin
          att_sensor(movement,response,code);
          movement = 0;
          break;
        case H_temp:
          //The dht_temperatures_sensor supports INFO and VALUE requests. he is used 
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
          //The bool_sensor supports INFO and STATE requests.
          bool_sensor(lamp,response,code);
          break;
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
TATUDevice device(DEVICE_NAME, ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get, set, bridge);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

// This is obrigatory, and defines this DEVICE
CREATE_DOD(DEVICE_NAME,
  ADD_SENSORS("luminosity", "ldr", "A3")
  ADD_SENSORS("move", "pir", "3")
  ADD_SENSORS("temp", "dht11", "8")
  ADD_SENSORS("gas", "mq2", "A0")
  ADD_SENSORS("sound", "mic", "A1")
  ADD_LAST_SENSOR("ar", "dht11", "8"),
  ADD_NONE()
);


//SETUP
void setup() {
  //In order to avoid problems caused by external interruptions,
  //is recomended disable the interruption when using the attachInterrupt function;
  Serial.println("Trying connect to the broker");  
  cli();//disable interruptions

  device.pub = &bridge;

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

