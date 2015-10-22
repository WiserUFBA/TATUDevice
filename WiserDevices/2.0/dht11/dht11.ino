#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

//Digital pin for the dht sensor
#define DHTPIN 8

//Dht sensor version
#define DHTTYPE DHT11  

//Port to connect with the broker 
#define MQTTPORT 1883

//Hash's that represents the attributes "temp" and "ar"
#define H_temp 2090755995
#define H_ar 5863224

#define MQTT_USER  "device"
#define MQTT_PASS  "boteco@wiser"

DHT dht(DHTPIN, DHTTYPE);

//variveis
int t,h,aux;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 127}; 

//int t,h,count1,count2;
unsigned long int time, lastConnect,prevTime,iTime;


bool get(uint32_t hash,void* response,uint8_t code){
  
  switch(hash){
      case H_temp
        t = (int)dht.readTemperature();
        switch(code){   
          case TATU_CODE_INFO;
            itoa(t,response,10);
            break;
          case TATU_CODE_VALUE;
            *response = t;
            break;
          default:
            return false;
        } 
        break;
      case H_ar
        h = (int)dht.readHumidity();
        switch(code){   
          case TATU_CODE_INFO;
            itoa(h,response,10);
            break;
          case TATU_CODE_VALUE;
            *response = h;
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

// Objects to example that uses ethernet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device("rele-pres", ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, get);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

void setup() {
  char aux[16];  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);  
  pinMode(DHTPIN,INPUT);

  //Trying connect to the broker  
  while(!client.connect(device.name,"device","boteco@wiser"));
  client.subscribe(device.name,1);

}
void loop() { client.loop(); 
  //Watchdog for connection with the broker
  if (!client.connected()) {
    reconnect();
  }
 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connect(device.name, MQTT_USER, MQTT_PASS)) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.publish("dev",device.name)) {
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
