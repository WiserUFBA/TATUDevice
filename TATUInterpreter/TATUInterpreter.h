#ifndef NewInterpreter_h
#define NewInterpreter_h

#define AVR_GCC

#include <stdint.h>

#ifndef AVR_GCC
#include <pgmspace.h>
#endif

#ifdef AVR_GCC
#include <avr/pgmspace.h>
#endif
// Uncomment the follow line to show debug
#define DEBUG
// Change debug port to Software Serial Object if you want to
//#define DEBUG_PORT                  Serial
#ifdef AVR_GCC
#define DEBUG_PORT                ATMSerial
#endif
// Allow Software Serial 
#define ENABLE_SOFTWARE_SERIAL

// If enabled Software Serial
#ifdef ENABLE_SOFTWARE_SERIAL
#include <SoftwareSerial.h>
// Software Serial should be static since this file can be called multiple times
SoftwareSerial static DEBUG_PORT(7,6);
#endif

// Debug Definitions
#define putstring(MSG)              SerialPrint_PROGMEM(MSG)
#define PRINT_DEBUG_PROGMEM(MSG)    SerialPrint_PROGMEM(MSG)
#define PRINT_DEBUG(MSG)            DEBUG_PORT.print(MSG)
#define PRINT_DEBUG_NL(MSG)         DEBUG_PORT.println(MSG)
#define DEBUG_NL()                  DEBUG_PORT.println()
#define DEBUG_PORT_SPEED            115200

// System definitions
#ifdef AVR_GCC
#define PROGMEM                     __ATTR_PROGMEM__
#endif

// TATU Protocol available commands
#define TATU_POST   0
#define TATU_SET    1
#define TATU_GET    2
#define TATU_FLOW   3
#define TATU_EDIT   4

// TATU Protocol available properties
#define TATU_CODE_DOD   0
#define TATU_CODE_INFO  1
#define TATU_CODE_VALUE 2
#define TATU_CODE_STATE 3

// TATU Protocol avaliable type variables
#define TATU_TYPE_ALIAS  0
#define TATU_TYPE_PIN    1
#define TATU_TYPE_SYSTEM 2
//#define TATU_TYPE_CRYPTO 3
#define TATU_TYPE_ANALOG 3

// Char that represents the TATU Protocol commands
#define CODE_DOD   'D'
#define CODE_INFO  'I'
#define CODE_FLOW  'F'
#define CODE_VALUE 'V'
#define CODE_STATE 'S'

// Char that represents the TATU Protocol properties
#define COMMAND_POST 'P'
#define COMMAND_SET  'S'
#define COMMAND_GET  'G'
#define COMMAND_EDIT 'E'

// DJB Hash macro(deprecated)
//#define HASH_DJB(START, LEN, INPUT, OUTPUT) for(i = START; INPUT[i] != ' '; i++){ OUTPUT = ((OUTPUT << 5) + OUTPUT) + INPUT[i]; }
// Copy property to begin of the payload array
#define VAR_COPY(START, LEN, VECTOR) for(i = START; i < LEN; i++){ VECTOR[i-START] = VECTOR[i]; if(i+1 == LEN) VECTOR[i-START+1] = '\0'; else if(VECTOR[i] == ' ') VECTOR[i-START] = 0; }
// Verifica se uma variavel é um numero
#define IS_NUM(VAR) ((VAR < 58)&&(VAR > 47))||(VAR == '#')
#define IS_SYS(VAR) (VAR == '$')
// Deprecated!
//#define IS_CRYPTO(VAR) (VAR == '#')
#define IS_ANALOG(VAR) (VAR == '#')

/* Utilities */
uint32_t hash_djb(char *string);
uint8_t atoi_T(char *p);

void SerialPrint_PROGMEM(PGM_P str);

class TATUInterpreter{
private:
    typedef union {
        struct {
            uint8_t ERROR : 1;
            uint8_t STATE : 1;
            uint8_t CODE  : 2;
            uint8_t TYPE  : 2;
            uint8_t VAR   : 2;
            uint8_t PIN      ;
        } OBJ;
        uint16_t STRUCTURE;
    } Command;
public:
    Command cmd;
    uint32_t str_hash;
    TATUInterpreter(){
        cmd.OBJ.ERROR = true;

        // Enable Software Serial Debug port if it's not already started 
        #ifdef ENABLE_SOFTWARE_SERIAL
        DEBUG_PORT.begin(DEBUG_PORT_SPEED);
        #endif
    }
    bool parse(char *, unsigned int);
};

#endif
