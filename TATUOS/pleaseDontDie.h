----------------------------------------------------
------------------ IMPLEMENTATION ------------------
----------------------------------------------------
---- SETUP ----
// Try to connect to the network 
while(!Ethernet.begin(mac,ip));
Serial.println("Successfuly connected to the network");

Serial.println("Trying to connect to the broker");

// Try to Connect to the broker
while(!client.connect(device.name)) 
Serial.println("The connection has failed");

Serial.println("The connection has suceeded");
client.subscribe(device.name);

---- LOOP ----
int i = 0, j = 0, z = 0;
echoReply = ping(pingAddr, 4);
if(echoReply.status != SUCCESS){
    while(i != 10){
        echoReply = ping(pingAddr, 4);
        if(echoReply.status == SUCCESS)
            break;
        i++;
        if(i == 10){
            client.disconnect();
            while(j != 10){
                if(client.connect(device.name)){
                    client.subscribe(device.name);
                    break;
                }
                j++;
                if(j == 10){
                    while(z != 10){
                        if(Ethernet.begin(mac,ip))
                            break;
                        z++;
                        if(z == 10){
                            wdt_reset();
                            while(true);
                        }
                    }
                    if(z != 10){
                        z = 0;
                        while(z != 10){
                            if(client.connect(device.name)){
                                client.subscribe(device.name);
                                break;
                            }
                            z++;
                            if(z == 10){
                                wdt_reset();
                                while(true);
                            }
                        }
                    }
                }
            } 
        }
    }
}

-----------------------------------------------------
-------------------- PSEUDO CODE --------------------
-----------------------------------------------------

---- SETUP ----
try to 'start communication'
failed?
.... try again - until 'success'
success?
.... try to 'start communicationBroker'
.... failed?
.... .... try again - until 'success'

... continue code ...

---- LOOP ----
try to 'ping $server'
failed?
.... try again - until '$i > 10'
.... failed?
.... .... try to 'restart communicationBroker'
.... .... failed?
.... .... .... try again - until '$i > 10'
.... .... .... failed ?
.... .... .... .... try to 'restart communication'
.... .... .... .... failed?
.... .... .... .... .... try again - until '$i > 10'
.... .... .... .... .... failed?
.... .... .... .... .... .... restartDevice
.... .... .... .... .... success?
.... .... .... .... .... .... try to 'restart communicationBroker' - until '$i > 10'
.... .... .... .... .... .... failed?
.... .... .... .... .... .... .... try to 'ping $server' - until '$i > 10' - otherwise 'restartDevice'
.... .... .... .... .... .... if noPreviousError continue - otherwise try again - until '$i > 10' - otherwise 'restartDevice'

... continue code ...
