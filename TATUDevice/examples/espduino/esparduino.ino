#include <stdint.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>

// To use with ESPDUINO by tuanpmt >> https://github.com/tuanpmt/espduino

#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>

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
#define DEVICE_NAME "test"
#define MQTT_USER   "wiser"
#define MQTT_PASS   "wiser2014"
#define MQTT_PORT   1883
#define MQTT_SERVER "192.168.0.120"

#define ESP_NETWORK "wiser"
#define ESP_PASS    "wiser2014"

#define on(PIN) digitalWrite(PIN,true)
#define off(PIN) digitalWrite(PIN,false)

// Message for annoucement of connection
char hello[] = DEVICE_NAME " has connected";

// System properties
byte server[] = { 192, 168, 25, 20 }; // ip do servidor
byte ip[]     = { 192, 168, 25, 40 }; // ip do dispositivo

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
            //Once the pointer to response is a void, there must be a cast
            ITOS(luminosity,response);// This macro uses a integer to reply a request for a string (ITOS = Integer to String)
            //That is what this macro actualy means: (itoa(luminosity,(char*)response,10))
            break;
          //VALUE means that the response must be a integer
          case TATU_CODE_VALUE:
            ITOI(luminosity,response);// This macro uses a integer to reply a request for a integer (ITOI = Integer to Integer)
            //That is what this macro actualy means: *(int*)INTEGER2 = INTEGER
            break;
          //STATE means that the response must be a boolean
          case TATU_CODE_STATE:
            return false;
            break;
          //A "false" return means that the device wasn't able to resolve the request
          default:
            return false;
        } 
        break;
      case H_lamp:
        switch(code){   
          case TATU_CODE_STATE:
            BTOB(lamp,response);// This macro uses a boolean to reply a request for a boolean (BTOB = Boolean to Boolean)
            //That is what this macro actualy means: *(bool*)response = lamp
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
  //SET STATE lamp T
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

// Coisas do ESP
SoftwareSerial debugPort(2, 3); // RX, TX
ESP esp(&Serial, &debugPort, 4);
MQTT mqtt(&esp);
boolean wifiConnected = false;

// Essential objects and macros
TATUInterpreter interpreter;
TATUDevice device(DEVICE_NAME, ip, 121, 88, 0, server, MQTT_PORT, 1, &interpreter, get, set);

/* MODIFICAR ESP */
void mqttSend(char *topic, char *out){
  mqtt.publish(topic,out);
}

//////////////////////// ESP /////////////////////////

void mqttReceived(void* response)
{
  RESPONSE res(response);

  debugPort.print("Received: topic=");
  String topic = res.popString();
  debugPort.println(topic);

  debugPort.print("data=");
  String data = res.popString();
  debugPort.println(data);

  char topico[30];
  strcpy(topico, topic.c_str());
  char mensagem[100];
  strcpy(mensagem, data.c_str());

  device.mqtt_callback(topico, (byte *)mensagem, data.length());
}

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      debugPort.println("WIFI CONNECTED");
      mqtt.connect(MQTT_SERVER, MQTT_PORT, false);
      wifiConnected = true;
    } else {
      wifiConnected = false;
      mqtt.disconnect();
    }
  }
}

void mqttConnected(void* response)
{
  debugPort.println("Connected");
  mqtt.publish("dev/CONNECTIONS", hello);
  mqtt.subscribe(device.aux_topic_name);
  mqtt.subscribe("dev");
}

void mqttDisconnected(void* response){}

void mqttPublished(void* response){}

//////////////////////////////////////////////////////

// Constructs the JSON that describes the device
CREATE_DOD(DEVICE_NAME,
  ADD_LAST_SENSOR("sensor_name", "type", "pin"),
  ADD_NONE()
);

/* Is just necessary to edit lines below if you modifies some variable */
void setup() { 
  device.pub = &mqttSend;

  Serial.begin(19200);
  debugPort.begin(19200);
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

  debugPort.println("ARDUINO: setup mqtt client");
  if(!mqtt.begin(DEVICE_NAME, MQTT_USER, MQTT_PASS, 1024, 1)) {
    debugPort.println("ARDUINO: fail to setup mqtt");
    while(1);
  }

  debugPort.println("ARDUINO: setup mqtt lwt");
  mqtt.lwt("/lwt", "TA LÁ O CORPO ESTENDIDO NO CHÃO!", 0, 0); 

  /*setup mqtt events */
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttReceived);
   
  // SETUP WIFI
  debugPort.println("ARDUINO: setup wifi");
  esp.wifiCb.attach(&wifiCb);

  esp.wifiConnect(ESP_NETWORK,ESP_PASS);
  debugPort.println("ARDUINO: system started");
}

void loop() { 
  esp.process();
}
