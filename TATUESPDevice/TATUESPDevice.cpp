#include "TATUESPDevice.h"


//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void TATUESPDevice::setup_wifi() {

    Serial.println("Starting Wifi Web Manager...");
    delay(10);

 // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_device_name("device", "device name", device_name, 15);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 20);
  WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", mqtt_pass, 20);
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", port, 5);

  WiFiManager wifiManager;
  wifiManager.setDebugOutput(false);

  //set config save notify callback
  void (*trampolin)(void) = (void (*)())saveConfigCallback;
  wifiManager.setSaveConfigCallback(trampolin);

  //add all your parameters here
  wifiManager.addParameter(&custom_device_name);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);

  wifiManager.startConfigPortal("TesteAP01", "wiser2014");
  //wifiManager.resetSettings();
  //wifiManager.autoConnect("UFBAinoAP", "wiser2014");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected to: " + WiFi.SSID() +
                    "pass: " + WiFi.psk());

  WiFi.mode(WIFI_STA);
  // wifiManager

  //read updated parameters
  strcpy(device_name, custom_device_name.getValue());
  strcpy(mqtt_user, custom_mqtt_user.getValue());
  strcpy(mqtt_pass, custom_mqtt_pass.getValue());
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(port, custom_mqtt_port.getValue());
  mqttport = atoi(port);

  //save the custom parameters to FS
  if (shouldSaveConfig && SPIFFS.begin()) {
    Serial.println("Saving config...");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["device_name"] = device_name;
    json["mqtt_user"] = mqtt_user;
    json["mqtt_pass"] = mqtt_pass;
    json["mqtt_server"] = mqtt_server;
    json["port"] = port;
    json["SSID"] = WiFi.SSID();
    json["pass"] = WiFi.psk();

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("Failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }
  device->init(device_name, 1, device->requisition);
  strcpy(&subsc_topic[4], device_name);

  client->setServer(mqtt_server, 1883);
  client->setCallback(mqtt_callback);

  client->subscribe("dev/");
  client->subscribe(subsc_topic);

  client->publish("dev/CONNECTIONS", device_name);

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void TATUESPDevice::search_config() {
  //read configuration from FS json

  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(device_name, json["device_name"]);
          strcpy(mqtt_user, json["mqtt_user"]);
          strcpy(mqtt_pass, json["mqtt_pass"]);
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(port, json["port"]);
          strcpy(ssid, json["SSID"]);
          strcpy(pass, json["pass"]);
          mqttport = atoi(port);
          if(WiFi.status() == WL_CONNECTED) return;
          WiFi.begin(ssid,pass);
          while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
          }

          Serial.println("");
          Serial.println("WiFi connected to: " + WiFi.SSID() +
                            "pass: " + WiFi.psk());
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
}
