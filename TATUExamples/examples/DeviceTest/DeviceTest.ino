#include <TATUInterpreter.h>
#include <TATUDevice.h>

#define MQTTPORT 1883

byte server[] = {192, 168, 0, 101};
byte null_ip[] = {0,0,0,0};

/* Get Unit Tests */
char test1[] = "GET VALUE 12";
char test2[] = "GET VALUE #12";
char test3[] = "GET VALUE $VACA";
char test4[] = "GET INFO 13";
char test5[] = "GET INFO #13";
char test6[] = "GET INFO $CHUPA_CABRA";
char test7[] = "GET STATE 13";
char test8[] = "GET STATE TREPADEIRA";
char test9[] = "GET STATE $LOL";
char test10[] = "GET ALL";

/* Set Unit Tests */
char test11[] = "SET VALUE 12 24";
char test12[] = "SET VALUE #13 250";
char test13[] = "SET VALUE CABRA 24";
char test14[] = "SET STATE 12 F";
char test15[] = "SET STATE LAMP T";
char test16[] = "SET STATE VALVE F";
char test17[] = "SET INFO VACA blahblah";
char test18[] = "SET INFO VACA 69ieie";

/* Edit Unit Tests */
char test19[] = "EDIT INFO IP 1.1.1.1";
char test20[] = "EDIT VALUE MQTT_PORT 6969";
char test21[] = "EDIT STATE RESET_ON T";
char test22[] = "EDIT STATE RESET_ON F";

/* Post Unit Tests */
char test23[] = "POST test:{}";
char test24[] = "POST algo:{HEADER:null,BODY:null}";

TATUInterpreter interpreter;
TATUDevice debugger("air", null_ip, 1, 2, 0, server, MQTTPORT, 1, &interpreter);
#define debug(DEVICE,STRING) \
    Serial.println(STRING);  \
    DEVICE.generateBody(STRING, strlen(STRING)); \
    Serial.println(DEVICE.output_message);\
    Serial.println("")

    
void setup(){
    Serial.begin(9600);
    
    Serial.println("=======================");
    Serial.println("Starting Debug...");
    Serial.println("=======================");
    Serial.println();
    
    // Parse TEST 1
    debug(debugger,test1);
    // Parse TEST 2
    debug(debugger,test2);
    // Parse TEST 3
    debug(debugger,test3);
    // Parse TEST 4
    debug(debugger,test4);
    // Parse TEST 5
    debug(debugger,test5);
    // Parse TEST 6
    debug(debugger,test6);
    // Parse TEST 7
    debug(debugger,test7);
    // Parse TEST 8
    debug(debugger,test8);
    // Parse TEST 9
    debug(debugger,test9);
    // Parse TEST 10
    debug(debugger,test10);
    // Parse TEST 11
    debug(debugger,test11);
    // Parse TEST 12
    debug(debugger,test12);
    // Parse TEST 13
    debug(debugger,test13);
    // Parse TEST 14
    debug(debugger,test14);
    // Parse TEST 15
    debug(debugger,test15);
    // Parse TEST 16
    debug(debugger,test16);
    // Parse TEST 17
    debug(debugger,test17);
    // Parse TEST 18
    debug(debugger,test18);
    // Parse TEST 19
    debug(debugger,test19);
    // Parse TEST 20
    debug(debugger,test20);
    // Parse TEST 21
    debug(debugger,test21);
    // Parse TEST 22
    debug(debugger,test22);
    // Parse TEST 23
    debug(debugger,test23);
    // Parse TEST 24
    debug(debugger,test24);

    Serial.println("=======================");
    Serial.println("Finished Debug!");
    Serial.println("=======================");
}

void loop(){}
