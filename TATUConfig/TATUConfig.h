#ifndef TATUConfig_h
#define TATUConfig_h


#include <FS.h>                    //this needs to be first, or it all crashes and burns...
#include <pgmspace.h>
#include <stdint.h>
#include <EEPROM.h>

#include <PubSubClient.h>

#include <TATUDevice.h>
// #include <TATUInterpreter.h>

//library configurationBrowser
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>           //https://github.com/bblanchon/ArduinoJson


//flag for saving data
static bool shouldSaveConfig = false;
void saveConfigCallback();

#define stringSize 20
class TATUConfig {
private:
    /* data */
public:
    char device_name[stringSize];
    char mqtt_user[stringSize];
    char mqtt_pass[stringSize];
    char port[stringSize];
    char mqtt_server[stringSize];
    char    subsc_topic[stringSize];
    uint8_t mqttport;

    PubSubClient* client;
    TATUDevice* device;
    // TATUDevice* device;

    // wifi
    char ssid[20];
    char pass[20];

    void (*mqtt_callback)(char *, byte *, unsigned int);

    TATUConfig (TATUDevice* device,PubSubClient* client,void (*mqtt_callback)(char *, byte *, unsigned int)){
        this->device = device;
        this->client = client;
        this->mqtt_callback = mqtt_callback;
    }
    void setup_wifi();
    void search_config();


};

#endif /* TATUConfig_h */
