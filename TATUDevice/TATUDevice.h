#ifndef TATUInterpreter_h
#define TATUInterpreter_h

#include "hash_list.h"
#include <stdint.h>

#define TATU_SET    0
#define TATU_GET    1
#define TATU_POST   2
#define TATU_EDIT   3   

#define TATU_CODE_ALL   0
#define TATU_CODE_INFO  1
#define TATU_CODE_PROP  2
#define TATU_CODE_STATE 3

#define CODE_ALL   'A'
#define CODE_INFO  'I'
#define CODE_PROP  'P'
#define CODE_STATE 'S'

#define COMMAND_SET  'S'
#define COMMAND_GET  'G'
#define COMMAND_POST 'P'
#define COMMAND_EDIT 'E'

#define PIN_AMOUNT 6

 //hash


#define HASH_DJB(START, LEN, INPUT, OUTPUT) for(i = START; i < LEN; i++){ OUTPUT = ((OUTPUT << 5) + OUTPUT) + INPUT[i]; }

//Calcula o comprimento da string
#define STRLEN(STRING,LENGTH) for(LENGTH = 0; STRING[LENGTH] != '\0';){LENGTH++;}

//Concatena duas strings
#define CONCAT(FIRST,SECOND,LENGTH) for(i = 0; SECOND[i] != '\0'; FIRST[LENGTH] = SECOND[i],i++,LENGTH++); FIRST[LENGTH] = '\0'
//Concatena uma string e um char
#define CONCATSINGLE(FIRST,SECOND,LENGTH) if(true) { FIRST[LENGTH++] = (char)SECOND; FIRST[LENGTH + 1] = '\0'; } 
//concatena um objeto com um elemento adicional
#define CONCATELEMENT(FIRST,SECOND,LENGTH) if(true) {CONCATSINGLE(FIRST,',',LENGTH);CONCAT(FIRST,SECOND,LENGTH);}
//concatena uma variavel com o seu valor
#define CONCATVALUE(FIRST,SECOND,LENGTH) if(true) {CONCATSINGLE(FIRST,':',LENGTH);CONCAT(FIRST,SECOND,LENGTH);}
//concatena três objetos na ordema passada
#define CONCATTHREE(FIRST,SECOND,THIRD,LENGTH) if(true) {CONCAT(FIRST,SECOND,LENGTH);CONCAT(SECOND,THIRD,LENGTH);}

//DEPRECATED FOR NOW
//#define CONCATNEST(FIRST,SECOND,NESTER,LENGTH) if(true) {CONCATSINGLE(FIRST,NESTER,LENGTH);CONCAT(FIRST,SECOND,LENGTH);CONCATSINGLE(FIRST,NESTER + 2,LENGTH);}

//Escreve seu elemento com o seu valor(usado no primeiro elemento)
#define WRITE(STRING,VAR,VALUE,ELEM_LENGTH,LENGTH) if (true){ STRLEN(VAR,ELEM_LENGTH); CONCATVALUE(VAR,VALUE,ELEM_LENGTH); CONCAT(STRING,VAR,LENGTH);}
//Adiciona um elemento com o seu valor(usado nos próximos elementos)
#define ELEMENTWRITE(STRING,VAR,VALUE,ELEM_LENGTH,LENGTH) if (true){ STRLEN(VAR,ELEM_LENGTH); CONCATVALUE(VAR,VALUE,ELEM_LENGTH); CONCATELEMENT(STRING,VAR,LENGTH);}

//Calcula o comprimento da string
class Device{
private:

    typedef union {
        struct {
            uint8_t STATE : 1;
            uint8_t CODE  : 2;
            uint8_t TYPE  : 2;
            uint8_t PIN   : 3;
        } OBJ;
        uint8_t STRUCTURE;
    } Command;
    //informções da EEPROM 
    

    
public:
    uint8_t PAN_ID;
    uint8_t RESET_TIMES;
    uint16_t START_TIME;
    uint8_t SAMPLE_RATE;
    uint8_t OS_VERSION;
    uint8_t DEVICE_ID;
    uint8_t pins_value[PIN_AMOUNT];
    char pins_alias[PIN_AMOUNT][6];
    char DEVICE_NAME[21];
    
    uint16_t MQTT_PORT;
    char DEVICE_IP[16];
    char MQTT_IP[16];
    //char *room;

    Command cmd;
    char msg[200] = DEVICE_NAME;
    void post();
    void setHeader();
};

#endif