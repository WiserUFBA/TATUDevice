#define DEBUG 1
#include <TATUInterpreter.h>


char test1[] = "GET VALUE 12";
char test2[] = "SET STATE LAMP T";
char test3[] = "EDIT INFO IP 1.1.1.1";
char test4[] = "POST test:{}";
char test5[] = "GET INFO #13";
char test6[] = "SET STATE VALVE F";
char test7[] = "GET STATE $LOL";
char test8[] = "POST algo:{HEADER:null,BODY:null}";

TATUInterpreter debugger;
#define debug(INTERPETER,STRING) \
    Serial.println(STRING); \
    debugger.parse(STRING, strlen(STRING)); \
    Serial.println(); \
    Serial.println(INTERPETER.cmd.OBJ.TYPE); \
    Serial.print(">> TATU TYPE: "); \
    Serial.println(INTERPETER.cmd.OBJ.TYPE); \
    Serial.print(">> TATU CODE: "); \
    Serial.println(INTERPETER.cmd.OBJ.CODE); \
    Serial.print(">> TATU VAR: "); \
    Serial.println(INTERPETER.cmd.OBJ.VAR); \
    Serial.print(">> TATU STATE: "); \
    Serial.println(INTERPETER.cmd.OBJ.STATE); \
    Serial.print(">> TATU PIN: "); \
    Serial.println(INTERPETER.cmd.OBJ.PIN); \
    Serial.println(); \
    Serial.println() \

void setup(){
    Serial.begin(9600);
    
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



}

void loop(){}
