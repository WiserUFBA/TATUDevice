#include "TATUDevice.h"
#include <TATUInterpreter.h>
#include <stdint.h>
#include <string.h>
#include "Arduino.h"

#ifdef AVR_GCC
#include <avr/pgmspace.h>
#endif

/* DEBUG! */
#ifdef DEBUG
const char GENERATE_BODY[]          PROGMEM = "[DEBUG] Generating Body";
const char CALLBACK_INFO[]          PROGMEM = "[DEBUG] INFO function is being executed";
const char CALLBACK_VALUE[]         PROGMEM = "[DEBUG] VALUE function is being executed";
const char CALLBACK_STATE[]         PROGMEM = "[DEBUG] STATE function is being executed";
const char GET_PIN[]                PROGMEM = "[DEBUG] The value of the pin is: ";
const char SET_PIN[]                PROGMEM = "[DEBUG] The value of the pin has been set";
const char THE_INFO_RESPONSE[]      PROGMEM = "[DEBUG] The response for the GET INFO requisition is: ";
const char THE_VALUE_RESPONSE[]     PROGMEM = "[DEBUG] The response for the GET VALUE requisition is: ";
const char THE_STATE_RESPONSE[]     PROGMEM = "[DEBUG] The response for the GET STATE requisition is: ";
const char BODY_GENERATED[]         PROGMEM = "[DEBUG] The body generation is done";
const char IS_A_POST[]              PROGMEM = "[DEBUG] It's a post so it doesn't have to publish anything";
const char PUBLISHING[]             PROGMEM = "[DEBUG] Publishing...";
const char PUBLISHED[]              PROGMEM = "[DEBUG] The message has been published";
const char NOT_A_GET[]              PROGMEM = "[DEBUG] It isn't a GET requisition";
const char SYSTEM[]                 PROGMEM = "[DEBUG] The system function isn't working yet";
const char THE_RESPONSE[]           PROGMEM = "[DEBUG] The value of the response is: ";
const char INITIATING[]             PROGMEM = "[DEBUG] Initianting the class...";
const char FINISHED_INIT[]          PROGMEM = "[DEBUG] Finished init!";
const char STARTING_GENERATE[]      PROGMEM = "[DEBUG] Starting generate HEADER...";
const char ENDING_GENERATE[]        PROGMEM = "[DEBUG] Finished generate HEADER!";
const char HEADER_STR[]             PROGMEM = "[DEBUG] HEADER Value : ";
const char CLASS_CONSTRUCTED[]      PROGMEM = "[DEBUG] Class constructed with success!";
const char EXEC_ERROR[]             PROGMEM = "[DEBUG] Execution Error!";
const char EXEC_ERROR_TYPE_VAR[]    PROGMEM = "[DEBUG] Unknown variable type!";
const char PARAM_ERROR[]            PROGMEM = "[DEBUG] Param Error!";
const char RESPONSE_TYPE_INFO[]     PROGMEM = "[DEBUG] The response type is INFO";
const char RESPONSE_TYPE_VALUE[]    PROGMEM = "[DEBUG] The response type is VALUE";
const char RESPONSE_TYPE_STATE[]    PROGMEM = "[DEBUG] The response type is STATE";
const char DOD_RETURN[]             PROGMEM = "[DEBUG] Returning the following DOD Object";
#endif

// Constantes
const char start_post[] PROGMEM = "{\"CODE\":\"POST\"";
const char null_body[]  PROGMEM = "\"BODY\":null}";
const char true_body[]  PROGMEM = "\"BODY\":true}";
const char false_body[] PROGMEM = "\"BODY\":false}";
const char header_str[] PROGMEM = "\"HEADER\":{";
const char name_str[]   PROGMEM = "\"NAME\":\"";
const char id_str[]     PROGMEM = "\"ID\":";
const char pan_str[]    PROGMEM = "\"PAN\":";
const char ip_str[]     PROGMEM = "\"IP\":\"";
const char body_str[]   PROGMEM = "\"BODY\":{";
const char method_str[] PROGMEM = "\"METHOD\":";
const char method_get[] PROGMEM = "GET";
const char method_set[] PROGMEM = "SET";
const char true_str[]   PROGMEM = "true";
const char false_str[]  PROGMEM = "false";
const char null_str[]   PROGMEM = "null";
const char pin_str[]    PROGMEM = "PIN";
const char dev_str[]    PROGMEM = "dev/";
const char int_str[]    PROGMEM = "/INT";
const char res_str[]    PROGMEM = "/RES";
const char flow_str[]   PROGMEM = "/FLOW";


/* Utilidades */
/* Get the actual free ram */
int freeRAM(){
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int)__brkval);
}
/* Convert a 4 bytes array to a string */
void ipToString(byte *ip, char *str){
    int i, j;
    for(i = 0, j = 0; i < 4; i++){ itoa(ip[i], &str[j], 10); j += strlen(&str[j]); str[j++]= '.'; }
    str[j-1] = 0;
}

/* Construct the TATUDevice Class with only the GET callback */
TATUDevice::TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
            const int sample_d, byte *ip_m, const int port_m, const int os_v,
            TATUInterpreter *req, bool (*GET_FUNCTION)(uint32_t hash, void* response, uint8_t code),
            void (*PUBLISH)(char *, char *)){
    get_function = GET_FUNCTION;
    set_function = NULL;
    pub = PUBLISH;
    init(name_d,ip_d,id_d,pan_d,sample_d,ip_m,port_m,os_v,req);
}

// > ONLY SET
TATUDevice::TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
            const int sample_d, byte *ip_m, const int port_m, const int os_v,
            TATUInterpreter *req, bool (*SET_FUNCTION)(uint32_t hash, uint8_t code, void* request),
            void (*PUBLISH)(char *, char *)){
    set_function = SET_FUNCTION;
    get_function = NULL;
    pub = PUBLISH;
    init(name_d,ip_d,id_d,pan_d,sample_d,ip_m,port_m,os_v,req);
}

// > BOTH
TATUDevice::TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
            const int sample_d, byte *ip_m, const int port_m, const int os_v,
            TATUInterpreter *req, bool (*GET_FUNCTION)(uint32_t hash, void* response, uint8_t code), 
            bool (*SET_FUNCTION)(uint32_t hash, uint8_t code, void* request),
            void (*PUBLISH)(char *, char *)){
    get_function = GET_FUNCTION;
    set_function = SET_FUNCTION;
    pub = PUBLISH;
    init(name_d,ip_d,id_d,pan_d,sample_d,ip_m,port_m,os_v,req);
}

// > NONE
TATUDevice::TATUDevice( const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
            const int sample_d, byte *ip_m, const int port_m, const int os_v,
            TATUInterpreter *req,
            void (*PUBLISH)(char *, char *)){
    get_function = NULL;
    set_function = NULL;
    pub = PUBLISH;
    init(name_d,ip_d,id_d,pan_d,sample_d,ip_m,port_m,os_v,req);
}

/* Initialize the class */
void TATUDevice::init(  const char *name_d, byte *ip_d, const int id_d,   const int pan_d,
                        const int sample_d, byte *ip_m, const int port_m, const int os_v,
                        TATUInterpreter *req){
    cli();
    int i;
    char aux[20];

    // Define os atributos básicos
    i = strlen(name_d);
    len_name = i + 4;
    STRCPY(name_d, name);

    strcpy_P(aux_topic_name,dev_str);
    strcpy(&aux_topic_name[strlen(aux_topic_name)],name_d);
    
    ipToString(ip_d, aux);
    STRCPY(aux, ip);
    id = (uint8_t)  id_d;
    pan = (uint8_t) pan_d;
    samples = (uint8_t) sample_d;
    ipToString(ip_m, aux);
    STRCPY(aux, mqtt_ip);
    mqtt_port = (uint16_t) port_m;
    os_version = (uint8_t) os_v;
    requisition = req;

    // Gera o header padrão e coloca no output_message atualizando a posição final do header
    generateHeader();
    sei();

    dod_used = false;

    // Enable Software Serial Debug port if it's not already started 
    #ifdef ENABLE_SOFTWARE_SERIAL
    DEBUG_PORT.begin(DEBUG_PORT_SPEED);
    #endif
}

/* Generate the header post */
void TATUDevice::generateHeader(){
    /* Auxiliary variable */
    int aux;
    char aux_str[10];

    // Primeiro se coloca a seguinte string padrão no vetor
    //strcpy(&output_message[strlen(start_post)], name);
    strcpy_P(output_message, start_post);
    
    // Inicia o JSON
    //aux = strlen(name) + strlen(start_post);
    aux = strlen(start_post);
    COMMA;
    
    // As próximas linhas produzem o HEADER
    strcpy_P(OUT_STR, header_str); /* Copia o HEADER */
    aux += 10;
    
    /* Coloca o NAME */
    strcpy_P(OUT_STR, name_str);
    aux += 8;
    strcpy(OUT_STR, name);
    aux += strlen(name);
    QUOTE; COMMA;

    /* Coloca o ID */
    strcpy_P(OUT_STR, id_str);
    aux += 5;
    itoa(id, aux_str, 10);
    strcpy(OUT_STR, aux_str);
    aux += strlen(aux_str);
    COMMA;
    
    /* Coloca o PAN */
    strcpy_P(OUT_STR, pan_str);
    aux += 6;
    itoa(pan, aux_str, 10);
    strcpy(OUT_STR, aux_str);
    aux += strlen(aux_str);
    // COMMA;
    
    /* Coloca o IP */
    // Comment this out to PUT IP on header
    /*
    strcpy_P(OUT_STR, ip_str);
    aux += 6;
    strcpy(OUT_STR, ip);
    aux += strlen(ip);
    QUOTE;
    */

    /* Fecha a mensagem de saída */
    BRACE_RIGHT; COMMA;
    CLOSE_MSG;

    last_char = aux;
}

/* Parse and generate the body for the POST message */
void TATUDevice::generateBody(char *payload, uint8_t length){
    #ifdef DEBUG
    PRINT_DEBUG_PROGMEM(GENERATE_BODY);
    DEBUG_NL();
    #endif
    
    if(dod_used){
        dod_used = false;
        generateHeader();
    }

    void *response,*request;
    int aux = last_char;
    bool isString,
         bool_buffer = false;
    char str_buffer[MAX_SIZE_RESPONSE] = {0};
    uint16_t int_buffer = 0;         

    // Se encontrados erros no PARSE retorne "BODY":null
    if(!requisition->parse(payload, length)){ strcpy_P(OUT_STR, null_body); return; }
    
    // DEPRECATED //
    if(requisition->cmd.OBJ.TYPE == TATU_POST){ strcpy_P(OUT_STR, false_body); return; }
    if (requisition->cmd.OBJ.VAR == TATU_TYPE_SYSTEM){
        #ifdef DEBUG
        PRINT_DEBUG_PROGMEM(SYSTEM);
        DEBUG_NL();
        #endif
        /** MODIFICA PROPRIEDADE **/
        // ISTO AINDA NÃO FOI IMPLEMENTADO
        strcpy_P(OUT_STR, false_body);
        return;
    }
    /* Check the variable type */
    switch(requisition->cmd.OBJ.TYPE){
        /* If the desired variable is of type ALIAS, call the user's function */
        
        case TATU_GET:
            #ifdef DEBUG
            //PRINT_DEBUG_PROGMEM(CALLBACK_GET);
            //DEBUG_NL();
            #endif

            if(requisition->cmd.OBJ.CODE == TATU_CODE_DOD){
                dod_used = true;
                strcpy_P(output_message, DOD);

                #ifdef DEBUG
                PRINT_DEBUG_PROGMEM(DOD_RETURN);
                DEBUG_NL();
                PRINT_DEBUG_PROGMEM(DOD);
                DEBUG_NL();
                #endif

                return;
            }

            switch(requisition->cmd.OBJ.VAR){
                case TATU_TYPE_ALIAS:
                    //Baseado no código da resposta, decide qual função do usuário deve ser usada
                    switch(requisition->cmd.OBJ.CODE) {
                        case TATU_CODE_INFO:
                            response = &str_buffer;
                            break;
                        case TATU_CODE_VALUE:
                            response = &int_buffer;
                            break;
                        case TATU_CODE_STATE:
                            response = &bool_buffer;
                            break;
                    }
                    requisition->cmd.OBJ.ERROR = !get_function(requisition->str_hash,response,requisition->cmd.OBJ.CODE);
                    break;
                /* GPIO Modifier */
                case TATU_TYPE_PIN:
                    //MODIFICAR!!!
                    switch(requisition->cmd.OBJ.CODE){
                        case TATU_CODE_INFO:
                            itoa(digitalRead(requisition->cmd.OBJ.PIN), str_buffer, 10);
                            break;
                        case TATU_CODE_VALUE:
                            int_buffer = digitalRead(requisition->cmd.OBJ.PIN);
                            break;
                        case TATU_CODE_STATE:
                            bool_buffer = digitalRead(requisition->cmd.OBJ.PIN);
                            break;
                    }
                    break;
                /* ADC Modifier */
                case TATU_TYPE_ANALOG:
                    //Baseado no código da resposta, decide como a resposta deve ser dada
                    switch(requisition->cmd.OBJ.CODE){
                        case TATU_CODE_INFO:
                            itoa(analogRead(requisition->cmd.OBJ.PIN), str_buffer, 10);
                            break;
                        case TATU_CODE_VALUE:
                            int_buffer = analogRead(requisition->cmd.OBJ.PIN);
                            break;
                        case TATU_CODE_STATE:
                            bool_buffer = analogRead(requisition->cmd.OBJ.PIN);
                            break;
                    }
                    break;
            }
            break;
        case TATU_SET:
            #ifdef DEBUG
            //PRINT_DEBUG_PROGMEM(CALLBACK_SET);
            //DEBUG_NL();
            #endif
            
            switch(requisition->cmd.OBJ.VAR){

                case TATU_TYPE_ALIAS:
                    
                    //Baseado no código da resposta, decide qual função do usuário deve ser usada
                    switch(requisition->cmd.OBJ.CODE) {
                        case TATU_CODE_INFO:
                            request = &payload[strlen(payload)+1];
                            break;
                        case TATU_CODE_VALUE:
                            int_buffer = atoi(&payload[strlen(payload)+1]);
                            request = &int_buffer;
                            break;
                        case TATU_CODE_STATE:
                            bool_buffer = requisition->cmd.OBJ.STATE;
                            request = &bool_buffer;
                            break;
                    }
                    requisition->cmd.OBJ.ERROR = !set_function(requisition->str_hash, requisition->cmd.OBJ.CODE, request);
                    break;
                /* GPIO Modifier */
                case TATU_TYPE_PIN:
                    digitalWrite(requisition->cmd.OBJ.PIN,requisition->cmd.OBJ.STATE);
                    requisition->cmd.OBJ.ERROR = false;
                    #ifdef DEBUG
                    PRINT_DEBUG_PROGMEM(SET_PIN);
                    DEBUG_NL();
                    #endif
                    break; 
                /* ADC Modifier */
                case TATU_TYPE_ANALOG:
                    analogWrite(requisition->cmd.OBJ.PIN, atoi(&payload[strlen(payload) + 1]));
                    requisition->cmd.OBJ.ERROR = false;
                    #ifdef DEBUG
                    PRINT_DEBUG_PROGMEM(SET_PIN);
                    DEBUG_NL();
                    #endif
                    break;
            }
            break;


        case TATU_FLOW:
            switch(requisition->cmd.OBJ.VAR){
                case TATU_TYPE_ALIAS:
                    //request is the json
                    request = &payload[strlen(payload)+1];
                    requisition->cmd.OBJ.ERROR = !flow_function(requisition->str_hash,requisition->cmd.OBJ.CODE,request);
                    break;
            }
            break;
        default:
            #ifdef DEBUG
            PRINT_DEBUG_PROGMEM(EXEC_ERROR);
            DEBUG_NL();
            PRINT_DEBUG_PROGMEM(EXEC_ERROR_TYPE_VAR);
            DEBUG_NL();
            #endif
            strcpy_P(OUT_STR, null_body);
            return;
    }

    /* Coloca METHOD na resposta */
    strcpy_P(OUT_STR, method_str);
    aux += 9;

    // Como são apenas suportados GET e SET ...
    QUOTE;
    switch(requisition->cmd.OBJ.TYPE){
        case TATU_GET:
            strcpy_P(OUT_STR, method_get);
            aux += 3;
            break;
        case TATU_SET:
            strcpy_P(OUT_STR, method_set);
            aux += 3;
            break;
    }
    QUOTE; COMMA,

    /* Coloca o BODY na resposta */
    strcpy_P(OUT_STR, body_str);
    aux += 8;

    // !IMPORTANT! Suporte para apenas uma variavel ''
    /* Copia a variavel vinda do payload */
    QUOTE; strcpy(OUT_STR, payload); aux += strlen(payload); QUOTE; COLON;
    
    // Se encontrado qualquer tipo de erro
    if(requisition->cmd.OBJ.ERROR){ 
        #ifdef DEBUG
        PRINT_DEBUG_PROGMEM(PARAM_ERROR);
        DEBUG_NL();
        #endif
        
        strcpy_P(OUT_STR, null_str);
        aux += 4;
        
        BRACE_RIGHT; BRACE_RIGHT;
        return;
    }
   
    // Se não temos um GET verificamos se o SET ou EDIT não apresentaram erro
    if(requisition->cmd.OBJ.TYPE != TATU_GET){
        #ifdef DEBUG
        PRINT_DEBUG_PROGMEM(NOT_A_GET);
        DEBUG_NL();
        #endif
        
        strcpy_P(OUT_STR, true_str);
        aux += 4;

        BRACE_RIGHT; BRACE_RIGHT;
        return;
    }

    /* Verifica o tipo de resposta esperada, e responde adequadamente*/
    switch(requisition->cmd.OBJ.CODE) {
        case TATU_CODE_INFO:
            if (str_buffer[0] == '[' ){
                strcpy(OUT_STR, str_buffer);
                aux+=strlen(str_buffer);
            }
            else {QUOTE; strcpy(OUT_STR, str_buffer); aux+=strlen(str_buffer); QUOTE;}
            #ifdef DEBUG
            PRINT_DEBUG_PROGMEM(RESPONSE_TYPE_INFO);
            DEBUG_NL();
            PRINT_DEBUG_PROGMEM(THE_RESPONSE);
            PRINT_DEBUG_NL(str_buffer);
            #endif
            break;
        case TATU_CODE_VALUE:
            itoa(int_buffer,str_buffer,10);
            strcpy(OUT_STR, str_buffer);
            aux+=strlen(str_buffer);
            #ifdef DEBUG
            PRINT_DEBUG_PROGMEM(RESPONSE_TYPE_VALUE);
            DEBUG_NL();
            PRINT_DEBUG_PROGMEM(THE_RESPONSE);
            PRINT_DEBUG_NL(str_buffer);
            #endif
            break;
        case TATU_CODE_STATE:
            if (bool_buffer)  strcpy_P(str_buffer, true_str);
            else strcpy_P(str_buffer, false_str);
            strcpy(OUT_STR, str_buffer);
            aux+=strlen(str_buffer);
            #ifdef DEBUG
            PRINT_DEBUG_PROGMEM(RESPONSE_TYPE_STATE);
            DEBUG_NL();
            PRINT_DEBUG_PROGMEM(THE_RESPONSE);
            PRINT_DEBUG_NL(str_buffer);
            #endif
            break;
    }
    // Fecha o JSON e a STRING
    BRACE_RIGHT; BRACE_RIGHT;
    CLOSE_MSG;
    #ifdef DEBUG
    PRINT_DEBUG_PROGMEM(BODY_GENERATED);
    DEBUG_NL();
    #endif
}

/* Function to abstract some low-level publishing action */
void TATUDevice::callback(char *topic, byte *payload, unsigned int length){
    /* Gera o body e publica o mesmo */
    generateBody((char *) payload, (uint8_t) length);

    if(requisition->cmd.OBJ.TYPE == TATU_POST){
        #ifdef DEBUG
        PRINT_DEBUG_PROGMEM(IS_A_POST);
        DEBUG_NL();
        #endif
        return;
    }
    #ifdef DEBUG
    PRINT_DEBUG_PROGMEM(PUBLISHING);
    PRINT_DEBUG_NL(output_message);
    #endif
    
    strcpy_P(&aux_topic_name[len_name],res_str);
    
    //publish the message
    //pub(name, output_message); //publish the message 
    pub(aux_topic_name, output_message);

    aux_topic_name[len_name] = 0;

    #ifdef DEBUG
    PRINT_DEBUG_PROGMEM(PUBLISHED);
    DEBUG_NL();
    #endif

}
void TATUDevice::interruption(const char *var_name, int var){
    int aux = last_char;
    char str_buffer[MAX_SIZE_RESPONSE];

    /* Coloca o BODY na resposta */ 
    strcpy_P(OUT_STR, body_str); 
    aux += 8; 
    QUOTE; strcpy(OUT_STR, var_name); aux += strlen(var_name); QUOTE; COLON;  /* Copia a variavel vinda do payload */
    itoa(var,str_buffer,10);
    strcpy(OUT_STR, str_buffer);
    aux+=strlen(str_buffer);
    BRACE_RIGHT; BRACE_RIGHT;  /* Fecha o JSON e a STRING */ 
    CLOSE_MSG; 
    strcpy_P(&aux_topic_name[len_name],int_str);
    //pub(name, output_message); //publish the message 
    pub(aux_topic_name, output_message); //publish the message 
    aux_topic_name[len_name] = 0;
    //RESPONSE_CONSTRUCT(var_name);
}
void TATUDevice::interruption(const char *var_name, char *var){
    int aux = last_char;

    /* Coloca o BODY na resposta */ 
    strcpy_P(OUT_STR, body_str); 
    aux += 8; 
    QUOTE; strcpy(OUT_STR, var_name); aux += strlen(var_name); QUOTE; COLON;  /* Copia a variavel vinda do payload */ 
    QUOTE; strcpy(OUT_STR, var); aux+=strlen(var); QUOTE;
    BRACE_RIGHT; BRACE_RIGHT;  /* Fecha o JSON e a STRING */ 
    CLOSE_MSG; 
    strcpy_P(&aux_topic_name[len_name],int_str);
    pub(aux_topic_name, output_message); //publish the message 
    aux_topic_name[len_name] = 0;
    //RESPONSE_CONSTRUCT(var_name);
}
void TATUDevice::interruption(const char *var_name, bool var){
    int aux = last_char;

    /* Coloca o BODY na resposta */ 
    strcpy_P(OUT_STR, body_str); 
    aux += 8; 
    QUOTE; strcpy(OUT_STR, var_name); aux += strlen(var_name); QUOTE; COLON;  /* Copia a variavel vinda do payload */ 
    BRACE_RIGHT; BRACE_RIGHT;  /* Fecha o JSON e a STRING */ 
    CLOSE_MSG; 
    strcpy_P(&aux_topic_name[len_name],int_str);
    pub(aux_topic_name, output_message); //publish the message 
    aux_topic_name[len_name] = 0;
   //RESPONSE_CONSTRUCT(var_name);
}

void TATUDevice::interrupt(const char *var_name, char *var){
    int aux = last_char;

    /* Coloca o BODY na resposta */ 
    strcpy_P(OUT_STR, body_str); 
    aux += 8; 
    QUOTE; strcpy(OUT_STR, var_name); aux += strlen(var_name); QUOTE; COLON;  /* Copia a variavel vinda do payload */ 
    QUOTE; strcpy(OUT_STR, var); aux+=strlen(var); QUOTE;
    BRACE_RIGHT; BRACE_RIGHT;  /* Fecha o JSON e a STRING */ 
    CLOSE_MSG; 
    strcpy_P(&aux_topic_name[len_name],int_str);
    pub(aux_topic_name, output_message); //publish the message 
    aux_topic_name[len_name] = 0;
}

void TATUDevice::loop(){
    //mqtt_client.loop();
}

#ifdef AVR_GCC
void TATUWatchDog::watchdogSetup(){
    cli();
    // disable all interrupts
    wdt_reset();
    // reset the WDT timer
    /*
    WDTCSR configuration:
    WDIE = 1: Interrupt Enable
    WDE = 1 :Reset Enable
    WDP3 = 0 :For 2000ms Time-out
    WDP2 = 1 :For 2000ms Time-out
    WDP1 = 1 :For 2000ms Time-out
    WDP0 = 1 :For 2000ms Time-out
    */
    // Enter Watchdog Configuration mode:
    WDTCSR |= (1<<WDCE) | (1<<WDE);
    // Set Watchdog settings:
    WDTCSR =      (1<<WDIE) 
                | (1<<WDE) 
                | (0<<WDP3) 
                | (1<<WDP2) 
                | (1<<WDP1) 
                | (1<<WDP0);
    sei();
}

void TATUWatchDog::loop(){
    /*time = millis();
    if (!client.connected()){
        client.connect(name);
    }
        
    if (time - lastConnect > reset_time) 
        wdt_reset();*/
}
#endif    
