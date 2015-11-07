//#define LIB <Servo.h>

#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <sensors.h>
#include <string.h>
#include <DHT.h>

// Pins used
#define LUMINOSITY A3
#define MOVE 3
#define DHTPIN 8
#define SOUND A1
#define GAS A0

// DHT TYPE
#define DHTTYPE 11

// Constants to connection with the broker
#define DEVICE_NAME "ufbaino"
#define MQTT_USER  "device"
#define MQTT_PASS  "boteco@wiser"
#define MQTTPORT 1883

//Hash that represents the attributes
#define H_sound 274653294
#define H_gas 193492480
#define H_temp 2090755995
#define H_move 2090515612
#define H_luminosity 1516126306
#define H_humid 261814908


// Message for annoucement of connection
const char hello[] PROGMEM = 
  DEVICE_NAME
  " has connected";

DHT dht(DHTPIN, DHTTYPE);

//variveis
volatile int soundReading,movement,gas_amount,t,h,luminosity;
int aux;
bool lamp;
char str[20];  
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 10, 41, 0, 10 };
byte ip[4]    = { 10, 41 , 0 , 97 };
  
  bool get(uint32_t hash,void* response,uint8_t code){
    switch(hash){
        case H_move:
          //The I_V_sensor supports INFO and VALUE requests for any integer variable.
          I_V_sensor(movement,response,code);
          movement = 0;
          break;
        case H_sound:
          //The I_V_analog_sensor supports INFO and VALUE requests for any integer variable.
          I_V_analog_sensor(SOUND,soundReading,response,code);
          break;
        case H_temp:
          //The dht_temperatures_sensor supports INFO and VALUE requests.
          dht_temperature_sensor(dht,t,response,code);
          break;
        case H_humid:
          //The dht_humidity_sensor supports INFO and VALUE requests.
          dht_humidity_sensor(dht,h,response,code);
          break;
        case H_luminosity:
          //The lumisity_sensor supports INFO and VALUE,requests. 
          luminosity_sensor(LUMINOSITY,luminosity,response,code);
          break;
        case H_gas:
          //The gas_sensor supports INFO,VALUE and STATE requests.
          gas_sensor(GAS,gas_amount,response,code);
          break;
        default:
          return false;
    }
    return true; 
  }

// Objects to example that uses ethernet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device(DEVICE_NAME, ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get);
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

  device.publish_test = &bridge;

  Serial.begin(9600);
  Ethernet.begin(mac, ip);  
  dht.begin();
  pinMode(DHTPIN,INPUT);
  pinMode(MOVE, INPUT);
  
  digitalWrite(MOVE, HIGH);
  attachInterrupt(1, mexeu, FALLING);

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
  
  interruption_luminosity();
}


//PERSONAL FUNCTIONS

void interruption_luminosity(){
  luminosity = (analogRead(LUMINOSITY) - 1023) * (-1);
  luminosity = map (luminosity,0,1023,0,100);
  //device.interruption("luminosity",luminosity,'<',50);
  INTERRUPTION_VALUE(device,"luminosity",luminosity,'<',50);
}

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

