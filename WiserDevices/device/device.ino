#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Adafruit_CC3000.h>
#include <ccspi.h>
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

//
#define INTTIME 1

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

// Network properties
#define WLAN_SSID       "wiser"           // cannot be longer than 32 characters!
#define WLAN_PASS       "wiser2014"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2
#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   2  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  7
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins

// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed


// Message for annoucement of connection
const char hello[] PROGMEM = 
  DEVICE_NAME
  " has connected";

DHT dht(DHTPIN, DHTTYPE); 

//variveis
volatile int soundReading,movement,gas_amount,t,h,luminosity;
int aux;
long long int time,nextTime;
char str[20]; 
byte mac[]   = {  0xFA, 0xCA, 0xAA, 0x6A, 0x1F, 0xBA, 0x36, 0x16 };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168 , 0 , 137 };
  
  bool get(uint32_t hash,void* response,uint8_t code){
    switch(hash){
        case H_move:
          //The I_V_sensor supports INFO and VALUE requests for any integer variable.
          att_sensor(movement,response,code);
          movement = 0;
          break;
        case H_sound:
          //The I_V_analog_sensor supports INFO and VALUE requests for any integer variable.
          sensor(SOUND,soundReading,response,code);
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
Adafruit_CC3000_Client wifiClient = Adafruit_CC3000_Client();
TATUInterpreter interpreter;
TATUDevice device(DEVICE_NAME, ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , wifiClient);
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
  //cli();//disable interruptions

  device.pub = &bridge;

  Serial.begin(9600);

  dht.begin();
  pinMode(DHTPIN,INPUT);
  pinMode(MOVE, INPUT);
  
  Serial.println("Inicializando!");
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP()) { delay(100); }  
 
  Serial.println(F("Initialization complete"));
  
  
  digitalWrite(MOVE, HIGH);
//  attachInterrupt(1, mexeu, FALLING);

  //Trying connect to the broker
  //Serial.println("Trying connect to the broker");  
  while(!client.connect(device.name,MQTT_USER,MQTT_PASS));
  client.publish("dev/CONNECTIONS",hello);
  client.subscribe(device.aux_topic_name);
  client.subscribe("dev");
  //sei();//unable interruptions
  Serial.println("Conected!!");
}

//LOOP
void loop() { 
  time = millis();
  client.loop(); 
  //Watchdog for connection with the broker
  if (!client.connected()) {
    reconnect();
  }
  if(time > nextTime ){
    interruption_luminosity();
    nextTime = time + (INTTIME*1000);
  }
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

