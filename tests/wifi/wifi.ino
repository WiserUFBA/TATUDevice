/****************************************************************
WebClient.ino
CC3000 WebClient Test
Shawn Hymel @ SparkFun Electronics
March 1, 2014
https://github.com/sparkfun/SFE_CC3000_Library

Manually connects to a WiFi network and performs an HTTP GET
request on a web page. Prints the contents of the page to
the serial console.

The security mode is defined by one of the following:
WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA, WLAN_SEC_WPA2

Hardware Connections:
 
 Uno Pin    CC3000 Board    Function
 
 +5V        VCC or +5V      5V
 GND        GND             GND
 2          INT             Interrupt
 7          EN              WiFi Enable
 10         CS              SPI Chip Select
 11         MOSI            SPI MOSI
 12         MISO            SPI MISO
 13         SCK             SPI Clock

Resources:
Include SPI.h, SFE_CC3000.h, and SFE_CC3000_Client.h

Development environment specifics:
Written in Arduino 1.0.5
Tested with Arduino UNO R3

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, please
buy us a round!

Distributed as-is; no warranty is given.
****************************************************************/

#include <SPI.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>

// Pins
#define CC3000_INT      2   // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       7   // Can be any digital pin
#define CC3000_CS       10  // Preferred is pin 10 on Uno

// Connection info data lengths
#define IP_ADDR_LEN     4   // Length of IP address in bytes
#define MQTTPORT 1883

// Constants
char ap_ssid[] = "Baixo";                  // SSID of network
char ap_password[] = "EuEsqueci";          // Password of network
unsigned int ap_security = WLAN_SEC_WPA2; // Security of network
unsigned int timeout = 30000;             // Milliseconds
char server[] = "www.example.com";        // Remote host site

// Global Variables
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client client = SFE_CC3000_Client(wifi);
void setup() {
  
  ConnectionInfo connection_info;
  int i;
  
  Serial.println("Tentando conectar ao wifi");
  if ( !wifi.init() || !wifi.connect(ap_ssid, WLAN_SEC_WPA2, ap_password, TIMEOUT_CC3000)) Serial.println("Wifi nao conectado");
  else Serial.println("wifi conectado");
  Serial.println("Done");
  
  // Make a TCP connection to remote host
  Serial.print("Performing HTTP GET of: ");
  Serial.println(server);
  if ( !client.connect(server, 80) ) {
    Serial.println("Error: Could not make a TCP connection");
  }
  
}
#define MQTTPORT 1883
#define ATTEMPTS 3
#define SIZE     10
#define TIMEOUT 2
PingReport ping;
boolean wifiConectado,conectado;
long unsigned int time,startTrialTime;
void loop() { 
  for(i = 0; wifi.netapp_ping_send(server, ATTEMPTS, SIZE, TIMEOUT) != CC3000_SUCCESS; i++){
    startTrialTime = millis();
    time = millis();
    conectado = false;
    wifiConectado = false;
    while(!conectado  && time - startTrialTime > 10){
      if(client.connect(server, MQTTPORT))
        conectado = true;
    }
    if (conectado)
      break;
    conectadoWifi = false;
    time = millis();
    startTrialTime = millis();
    while(!conectado  && time - startTrialTime > 10){
      wifi.connect(ap_ssid, WLAN_SEC_WPA2, ap_password, TIMEOUT_CC3000)
        conectado = true;
    }
  }
    
}
