#include <stdint.h>
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>

// Atributte hashs
#define H_lamp 2090464143
#define H_luminosity 1516126306

// Device pins
#define LAMP 8
#define LUMINOSITY A3

// Device constants
#define DEVICE_SAMPLE 0
#define DEVICE_ID 121
#define DEVICE_PAN_ID 88

// Constants to connection with the broker
#define DEVICE_NAME ""
#define MQTT_USER  ""
#define MQTT_PASS  ""
#define MQTTPORT 1883

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

#define on(PIN) digitalWrite(PIN,true)
#define off(PIN) digitalWrite(PIN,false)

// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed

// Message for annoucement of connection
const char hello [] PROGMEM = 
{
  DEVICE_NAME \
  " has connected" \
};

// System properties
byte ip[4]    = { 0 }; // Null array to avoid erros
IPAddress server(192, 168, 0, 101); // If you aren't using the patched version of the CC3000 library, so put the IP in reverse order
byte server_b[4]    = { 192, 168, 0, 101 };

bool lamp;
int luminosity;

// The get function is called when a information is required
bool get(uint32_t hash,void* response,uint8_t code){
  
  // With the hash we know what atributte
  switch(hash){
      case H_luminosity:
        luminosity = analogRead(LUMINOSITY);
        //Using the code we know what is the type of the information required
        switch(code){
          //INFO means that the response must be a string 
          case TATU_CODE_INFO:
            ITOS(luminosity,response);// This macro uses a integer to reply a request for a string  
            break;
          //VALUE means that the response must be a integer
          case TATU_CODE_VALUE:
            ITOI(luminosity,response);// This macro uses a integer to reply a request for a integer
            break;
          //STATE means that the response must be a boolean
          case TATU_CODE_STATE:
            break;
          default:
            return false;
        } 
        break;
      case H_lamp:
        switch(code){   
          case TATU_CODE_STATE:
            BTOB(lamp,response);// This macro uses a boolean to reply a request for a boolean
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

// Objects to example that uses cc_3000
Adafruit_CC3000_Client wifiClient = Adafruit_CC3000_Client();
TATUInterpreter interpreter;
TATUDevice device("device", ip, 121, 88, 0, server_b, MQTTPORT, 1, &interpreter, get);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , wifiClient);
MQTT_PUBLISH(bridge, client);

CREATE_DOD("device_name",
  ADD_LAST_SENSOR("sensor_name", "type", "pin"),
  ADD_NONE()
);


/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
    
  Serial.println("Inicializando!");
  char aux[16];  
  Serial.begin(9600);  
  
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

  //Trying connect to the broker  
  while(!client.connect(device.name,MQTT_USER,MQTT_PASS));
  client.publish("dev/CONNECTIONS",hello);
  client.subscribe(device.aux_topic_name);
  client.subscribe("dev");

  Serial.println("Conected");
  
}
void loop() { 
  client.loop(); 
  if (!client.connected()) {
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
