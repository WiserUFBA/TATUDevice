#include <stdint.h>
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>

// EDIT MAX PACKET SIZE AND
// ENABLE MQTT_MAX_TRANSFER

#define H_lamp        2090464143
#define H_luminosity  1516126306

#define LUMINOSITY A3

// Device constants
#define DEVICE_SAMPLE 0
#define DEVICE_ID 121
#define DEVICE_PAN_ID 88

// Constants to connection with the broker
#define MQTT_USER  "device"
#define MQTT_PASS  "boteco@wiser"
#define MQTTPORT 1883

// Network properties
#define WLAN_SSID       "PiratasDoValeDoCanela"           // cannot be longer than 32 characters!
#define WLAN_PASS       "naovaqueebarril"
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

// System properties
byte ip[4]        = { 0 }; // Null array to avoid erros
byte server_b[4]  = { 192, 168, 10, 20 };
IPAddress         server(192, 168, 10, 20 ); // If you aren't using the patched version of the CC3000 library, so put the IP in reverse order

bool lamp;
int luminosity;

bool get(uint32_t hash,void* response,uint8_t code){
    switch(hash){
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
        case H_lamp:
            switch(code){   
                case TATU_CODE_INFO:
                    //if(lamp) ITOS("ON",response);
                    //else ITOS("OFF",response);
                    break;
                case TATU_CODE_STATE:
                    BTOB(lamp,response);
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

CREATE_DOD("device",
    ADD_LAST_SENSOR("ar", "dht11", "3"),
    ADD_NONE()
);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
        
    //cli();
    Serial.println("Inicializando!");
    device.publish_test = &bridge;
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
    if(!client.connect(device.name,MQTT_USER,MQTT_PASS)){
        Serial.println(F("Failed to connect with the broker"));
        while(1);
    }
    client.subscribe(device.aux_topic_name);
    client.subscribe("dev");
    //sei();
    Serial.println(F("Conected"));
    
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
        Serial.print(F("Attempting MQTT connection..."));
        // Attempt to connect
        if (client.publish("dev",device.name)) {
            Serial.println(F("connected"));
        } 
        else {
            Serial.print(F("failed, rc="));
            Serial.print(client.state());
            Serial.println(F(" try again in 5 seconds"));
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
