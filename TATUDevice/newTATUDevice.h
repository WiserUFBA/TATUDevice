#ifndef TATUDevice_h
#define TATUDevice_h

#include <stdint.h>
#include "hash_list.h"

#ifndef PIN_AMOUNT
#define PIN_AMOUNT 6
#endif

// Calcula o HASH DJB 
#define HASH_DJB(START, LEN, INPUT, OUTPUT) for(i = START; i < LEN; i++){ OUTPUT = ((OUTPUT << 5) + OUTPUT) + INPUT[i]; }
// Calcula o comprimento da string
#define STRLEN(STRING,LENGTH) for(LENGTH = 0; STRING[LENGTH] != '\0';){LENGTH++;}
// Copiar uma string para a outra
#define STRCPY(INPUT, OUTPUT) do{ for(i = 0;INPUT[i] != 0; ++i) OUTPUT[i] = INPUT[i]; }while(0)

class TATUDevice{
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

    typedef struct {
    	char 	 value[6];
    	uint32_t hash_value;
    } PinStruct;
public:
	// Atributos públicos
	// Atributos do sistema
	char 	 device_name[20];
	char 	 device_ip[16];
	uint8_t  device_id;
	uint8_t  device_pan;
	uint8_t  device_samples;
	char	 mqtt_ip[16];
	uint16_t mqtt_port;
	uint8_t  os_version;

	// Atributos variaveis
	uint8_t	  header_final_pos = 0;
	uint8_t	  pin_value[PIN_AMOUNT] = {0};
	PinStruct pin_alias[PIN_AMOUNT];

	/* TEORICO */
	/* uint8_t reset_counter;
	uint8_t start_counter; */

	// Mensagem de saida
	char output_msg[200] = {0};

	// Methodos públicos
	TATUDevice();
	void generateHeader();
	void generateBody();
};

#endif