#include <TATUDevice.h>
#include <PubSubClient.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
#i

while(!Ethernet.begin(mac,ip));
Serial.println("Successfuly connected to the network");

Serial.println("Trying to connect to the broker");

// Try to Connect to the broker
while(!client.connect(device.name)) 
Serial.println("The connection has failed");

Serial.println("The connection has suceeded");
client.subscribe(device.name);
#define CLIENTRECONNECT (X,DEVICE,CLIENT)for (X = 0; X < 10; X++){if(CLIENT.connect(DEVICE.name)){ wdt_reset(); CLIENT.subscribe(DEVICE.name); return;}}
---- LOOP ----
void wifiLoop(){
    int i = 0, j = 0, z = 0,count,aux;
    if (client.connected())
      return;
    for (i < 0; i < 10; i++){
        Serial.println("Checking connection with broker");
        for (count = 0; (count < 10) && !client.connected() ; count++);
        if (count < 10){ Serial.println("Conected with broker"); wdt_reset(); return;}

        client.disconnect();
        for (j = 0 ; j < 10 ; j++){
            Serial.println("Connecting with broker");
            for (count = 0;(count < 10) && (!client.connect(device.name,MQTTUSER,MQTTPASS))  ; count++);
            if (count < 10){ Serial.println("Conected with broker"); wdt_reset(); return;}
            Serial.println("Did'nt connected with broker");
            Serial.println("Tentando conectar ao wifi");
            for (count = 0; (count < 5) && (!wifi.connect(ap_ssid, WLAN_SECURITY, ap_password, TIMEOUT_CC3000)) ; count++);
            if (count < 5){
                Serial.println("Wifi conectado");
                Serial.println("Connecting with broker");
                for (aux = 0; (aux < 10) && (!client.connect(device.name,MQTTUSER,MQTTPASS)) ; aux++);
                if (aux < 10){ Serial.println("Conected with broker"); wdt_reset(); return;}
                Serial.println("Did'nt connected with broker");
                Serial.println("Resetando");
                return;
            }
            Serial.println("Did'nt connected with wifi");
        }
    }
}
ethernetLoop(){
    int i = 0, j = 0, z = 0,count,aux;
    echoReply = ping(pingAddr, 4);
    for (i < 0; i < 10; i++){

        for (count = 0; echoReply.status != SUCCESS && (count < 10) ; count++);
        if (count < 10){ wdt_reset(); return;}

        client.disconnect();

        for (j = 0 ; j < 10 ; j++){

            for (count = 0; (!client.connect(device.name)) && (count < 10) ; count++);
            if (count < 10){ wdt_reset(); return;}

            for (z = 0; z < 10; z++)
            {
                
                for (count = 0; (!Ethernet.begin(mac,ip)) && (count < 10) ; count++);
                    if (count < 10){
                        for (aux = 0; (!client.connect(device.name)) && (aux < 10) ; aux++);
                        if (aux < 10){ wdt_reset(); return;}

                    }
            }
        }
    }
}

