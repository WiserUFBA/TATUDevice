#include <stdint.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>

// Hash that represents a atributte
#define H_lamp 2090464143
#define H_luminosity 1516126306

// Device constants
#define DEVICE_SAMPLE 0
#define DEVICE_ID 121
#define DEVICE_PAN_ID 88

// Device pins
#define LAMP 8
#define LUMINOSITY A0

// Information for connection with broker
#define DEVICE_NAME ""
#define MQTT_USER  ""
#define MQTT_PASS  ""
#define MQTTPORT 1883

#define on(PIN) digitalWrite(PIN,true)
#define off(PIN) digitalWrite(PIN,false)

// Message for annoucement of connection
const char hello[] PROGMEM = DEVICE_NAME " has connected";

// System properties
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 25, 20 };
byte ip[]     = { 192, 168, 25, 40 };

bool lamp;
int luminosity;

// The get function is called when a information is required
bool get(uint32_t hash,void* response,uint8_t code){
  
  // With the hash we know what atributte
  switch(hash){
      case H_luminosity:
        luminosity = analogRead(LUMINOSITY);
        //Using the "code" variable we know what is the type of the information required
        switch(code){
          //INFO means that the response must be a string 
          case TATU_CODE_INFO:
            ITOS(luminosity,response);// This macro uses a integer to reply a request for a string  
            //That is what this macro actually means: (itoa(luminosity,(char*)response,10))
            break;
          //VALUE means that the response must be a integer
          case TATU_CODE_VALUE:
            ITOI(luminosity,response);// This macro uses a integer to reply a request for a integer
            //That is what this macro actually means: *(int*)response = luminosity
            break;
          //STATE means that the response must be a boolean
          case TATU_CODE_STATE:
            return false;
            break;
          //A "false" return means that the device was'nt able to resolve the request
          default:
            return false;
        } 
        break;
      case H_lamp:
        switch(code){   
          case TATU_CODE_STATE:
            BTOB(lamp,response);// This macro uses a boolean to reply a request for a boolean
            //That is what this macro actually means: *(bool*)response = lamp
            break;
          default:
            return false;
        } 
        break;
      default:
        return false;
  }
  return true; 
}
// The set function is called when a modification is required
// Notice that the order of the parameters is diferent from the get function
bool set(uint32_t hash, uint8_t code,void* request){
  
  // With the hash we know what atributte
  switch(hash){
      case H_luminosity:
        switch(code){
          default:
            return false;
        } 
        break;
      case H_lamp:
        switch(code){   
          case TATU_CODE_STATE:
            // Using the value of the request, we can change the value of a atrubutte and/or interact with a actuator
            if((bool*)request){ on(LAMP);lamp = true; }
            else { off(LAMP); lamp = false; }
            break;
          default:
            return false;
        } 
        break;
      default:
        return false;
  }
  return true; 
}

// Essential objects and macros
EthernetClient ethClient;
TATUInterpreter interpreter;
TATUDevice device(DEVICE_NAME, ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get, set);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , ethClient);
MQTT_PUBLISH(bridge, client);

// Constructs the JSON that describes the device
CREATE_DOD(DEVICE_NAME,
  ADD_LAST_SENSOR("sensor_name", "type", "pin"),
  ADD_NONE()
);

/* Is just necessary to edit lines below if you modifies some variable*/
void setup() { 
  
  char aux[16];  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);  

  //Trying connect to the broker  
  while(!client.connect(device.name,MQTT_USER,MQTT_PASS));
  client.publish("dev/CONNECTIONS",hello);
  client.subscribe(device.aux_topic_name);
  client.subscribe("dev");

  Serial.println("Conected");
 
  
}
void loop() { 
  client.loop(); 
  if(!client.connected()){
    reconnect();
  }
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
