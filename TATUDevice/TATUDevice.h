#ifndef TATUDevice_h
#define TATUDevice_h

#include <stdint.h>
#include <TATUConfig.h>
#include <TATUInterpreter.h>

#ifndef PIN_AMOUNT
#define PIN_AMOUNT 6
#endif

#ifndef PIN_ANALOG_AMOUNT
#define PIN_ANALOG_AMOUNT 6
#endif

#ifndef MAX_SIZE_RESPONSE
#define MAX_SIZE_RESPONSE 20
#endif

// Constantes do sistema
#define PROGMEM __ATTR_PROGMEM__ 
#define SAIDA_STR &output_message[aux]
// Constantes da mensagem
#define COMMA       output_message[aux++]=','
#define COLON       output_message[aux++]=':'
#define QUOTES      output_message[aux++]='\"'
#define BRACE_LEFT  output_message[aux++]='{'
#define BRACE_RIGHT output_message[aux++]='}'
#define CLOSE_MSG   output_message[aux]=0

// Calcula o HASH DJB 
#define HASH_DJB(START, LEN, INPUT, OUTPUT) for(i = START; i < LEN; i++){ OUTPUT = ((OUTPUT << 5) + OUTPUT) + INPUT[i]; }
// Copiar uma string para a outra
#define STRCPY(INPUT, OUTPUT) do{ for(i = 0;INPUT[i] != 0; ++i) OUTPUT[i] = INPUT[i]; OUTPUT[i] = 0; }while(0)
// Gera o body tendo o OBJETO dispositivo
#define generatePost(DEVICE) do{ DEVICE.generateHeader(); DEVICE.generateBody(); }while(0)
// Verifica se uma STRING está vazia
#define ISEMPTY(VAR) (VAR[0] == 0)

class TATUDevice{
private:
    // Atributos variaveis
    bool        pin_digital[PIN_AMOUNT];
    uint16_t    pin_analog[PIN_ANALOG_AMOUNT];
public:
    // Atributos públicos
    // Atributos do sistema
    char     device_name[20];
    char     device_ip[16];
    uint8_t  device_id;
    uint8_t  device_pan;
    uint8_t  device_samples;
    char     mqtt_ip[16];
    uint16_t mqtt_port;
    uint8_t  os_version;

    // Atributos variaveis
    TATUInterpreter *requisition;

    /* TEORICO */
    /* uint8_t reset_counter;
    uint8_t start_counter; */

    // Mensagem de saida
    char output_message[200];
    int last_char;

    // Methodos públicos
    TATUDevice( const char *,  const char *, const uint8_t,  const uint8_t,
                const uint8_t, const char *, const uint16_t, const uint8_t,
                TATUInterpreter *);
    void generateHeader();
    void generateBody(char *payload, uint8_t length, void (*callback)(uint32_t, char*)));
};

#endif