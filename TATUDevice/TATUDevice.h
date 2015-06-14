#ifndef TATUDevice_h
#define TATUDevice_h

#include <stdint.h>
#include <TATUInterpreter.h>
#include "hash_list.h"
typedef uint8_t byte;

#ifndef MAX_SIZE_RESPONSE
#define MAX_SIZE_RESPONSE   20
#endif

#ifndef MAX_SIZE_OUTPUT
#define MAX_SIZE_OUTPUT     200
#endif

#define DEBUG 1

// System definitions
#define PROGMEM             __ATTR_PROGMEM__
#define putstring(x)        SerialPrint_PROGMEM(x)
#define PRINT_DEBUG(MSG)    SerialPrint_PROGMEM(MSG)
#define DEBUG_NL            Serial.write('\n')
#define OUT_STR &output_message[aux]
#define MAX_SIZE_IP     16
#define MAX_SIZE_NAME   20

// Constantes da mensagem
#define COMMA       output_message[aux++]=','
#define COLON       output_message[aux++]=':'
#define QUOTE       output_message[aux++]='\"'
#define BRACE_LEFT  output_message[aux++]='{'
#define BRACE_RIGHT output_message[aux++]='}'
#define CLOSE_MSG   output_message[aux]=0

// Calcula o HASH DJB (deprecated)
//#define HASH_DJB(START, LEN, INPUT, OUTPUT) for(i = START; i < LEN; i++){ OUTPUT = ((OUTPUT << 5) + OUTPUT) + INPUT[i]; }
// Copiar uma string para a outra
#define STRCPY(INPUT, OUTPUT) do{ for(i = 0;INPUT[i] != 0; ++i) OUTPUT[i] = INPUT[i]; OUTPUT[i] = 0; }while(0)
// Gera o body tendo o OBJETO dispositivo
#define generatePost(DEVICE) do{ DEVICE.generateHeader(); DEVICE.generateBody(); }while(0)
// Verifica se uma STRING está vazia
#define ISEMPTY(VAR) (VAR[0] == 0)
// Connecta o dispostivo ao client MQTT
#define DEVICECONNECT(CLIENT,DEVICE) if(CLIENT.connect(DEVICE.name))CLIENT.subscribe(DEVICE.name)
// Cria wrapper para a função de callback da classe
#define MQTT_CALLBACK(BRIDGE,OBJ, NAME) void BRIDGE(char *, char *);\
                                        void NAME(char *topic, byte *payload, unsigned int length) \
                                        {OBJ.mqtt_callback(topic, payload, length, BRIDGE);}
#define MQTT_PUBLISH(BRIDGE, OBJ) void BRIDGE(char *topic, char *out)\
                                  { OBJ.publish(topic,out); }
typedef struct 
    {
        //INFO
        bool (*info)(uint32_t, char*, char*, uint8_t);
        //VALUE
        bool (*value)(uint32_t, uint16_t*, uint16_t, uint8_t);
        //STATE
        bool (*state)(uint32_t, bool*, bool, uint8_t);
}Callback;

//Essas funções tem como objetivo serem usadas como padrão, quando elas não são definidas pelo usuário
//INFO
bool info_default(uint32_t, char*, char*, uint8_t);
//VALUE
bool value_default(uint32_t, uint16_t*, uint16_t, uint8_t);
//STATE
bool state_default(uint32_t, bool*, bool, uint8_t);

/* Utilidades */
int freeRAM();
void ipToString(byte *ip, char *str);

class TATUDevice{
public:
    // Atributos públicos
    // Atributos do sistema
    char     name[MAX_SIZE_NAME];
    char     ip[MAX_SIZE_IP];
    uint8_t  id;
    uint8_t  pan;
    uint8_t  samples;
    char     mqtt_ip[MAX_SIZE_IP];
    uint16_t mqtt_port;
    uint8_t  os_version;

    // Atributos variaveis
    TATUInterpreter *requisition;

    /* TEORICO */
    /* uint8_t reset_counter;
    uint8_t start_counter; */

    // Mensagem de saida
    char output_message[MAX_SIZE_OUTPUT];
    int last_char;

    /* Callback's do Sistema */
    // Callback MQTT
    void mqtt_callback(char *, byte *, unsigned int, void (*publish)(char *, char *));
    // Callback Criado pelo usuario
    Callback TATUCallback;
    bool (*callback)(uint32_t, char*, char*, uint8_t);
    // Metodos públicos
    TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
                const int sample_d, byte *ip_m, const int port_m, const int os_v,
                TATUInterpreter *req,Callback callback_struct);

    TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
                const int sample_d, byte *ip_m, const int port_m, const int os_v,
                TATUInterpreter *req, bool (*callback_con)(uint32_t, char*, char*, uint8_t));
     TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
                const int sample_d, byte *ip_m, const int port_m, const int os_v,
                TATUInterpreter *req, bool (*callback_con)(uint32_t, uint16_t*, uint16_t, uint8_t));
      TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
                const int sample_d, byte *ip_m, const int port_m, const int os_v,
                TATUInterpreter *req, bool (*callback_con)(uint32_t, bool*, bool, uint8_t));

    void init( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
            const int sample_d, byte *ip_m, const int port_m, const int os_v,
            TATUInterpreter *req); 
    

    void generateHeader();
    void generateBody(char *payload, uint8_t length);

};

#endif
