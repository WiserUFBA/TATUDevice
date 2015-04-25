#include "TATUDevice.h"
#include "hash_list.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

// Constantes
const char start_post[] PROGMEM = "POST ";
const char null_body[]  PROGMEM = "\"BODY\":null}";
const char true_body[]  PROGMEM = "\"BODY\":true}";
const char header_str[] PROGMEM = "\"HEADER\":{";
const char name_str[]   PROGMEM = "\"NAME\":\"";
const char id_str[]     PROGMEM = "\"ID\":";
const char pan_str[]    PROGMEM = "\"PAN\":";
const char ip_str[]     PROGMEM = "\"IP\":\"";
const char body_str[]   PROGMEM = "\"BODY\":{";
const char true_str[]   PROGMEM = "true";
const char false_str[]  PROGMEM = "false";
const char pin_st[]     PROGMEM = "PIN";

TATUDevice::TATUDevice( const char *name_d,     const char *ip_d, const uint8_t id_d,    const uint8_t pan_d,
                        const uint8_t sample_d, const char *ip_m, const uint16_t port_m, const uint8_t os_v){
    int i;

    // Define os atributos básicos
    STRCPY(name_d, device_name);
    STRCPY(ip_d, device_ip);
    device_id = id_d;
    device_pan = pan_d;
    device_samples = sample_d;
    STRCPY(ip_m, mqtt_ip);
    mqtt_port = port_m;
    os_version = os_v;

    // Gera o header padrão e coloca no output_message atualizando a posição final do header
    generateHeader();
}

void TATUDevice::generateHeader(){
    /* Auxiliary variable */
    int aux;
    char aux_str[10];

    // Primeiro se coloca a seguinte string padrão no vetor
    strcpy_P(output_message, start_post);
    strcpy(&output_message[5], device_name);
    
    // Inicia o JSON
    aux = strlen(device_name) + 5;
    COLON;
    BRACE_LEFT;
    
    // As próximas linhas produzem o HEADER
    strcpy_P(SAIDA_STR, header_str); /* Copia o HEADER */
    aux += 10;
    
    /* Coloca o NAME */
    strcpy_P(SAIDA_STR, name_str);
    aux += 8;
    strcpy(SAIDA_STR, device_name);
    aux += strlen(device_name);
    QUOTE;
    COMMA;

    /* Coloca o ID */
    strcpy_P(SAIDA_STR, id_str);
    aux += 5;
    itoa(device_id, aux_str, 10);
    strcpy(SAIDA_STR, aux_str);
    aux += strlen(aux_str);
    COMMA;
    
    /* Coloca o PAN */
    strcpy_P(SAIDA_STR, pan_str);
    aux += 6;
    itoa(device_pan, aux_str, 10);
    strcpy(SAIDA_STR, aux_str);
    aux += strlen(aux_str);
    COMMA;
    
    /* Coloca o IP */
    strcpy_P(SAIDA_STR, ip_str);
    aux += 6;
    strcpy(SAIDA_STR, device_ip);
    aux += strlen(device_ip);
    QUOTE;

    /* Fecha a mensagem de saída */
    BRACE_RIGHT;
    COMMA;
    CLOSE_MSG;

    last_char = aux;
}

                                                                               /* Hash | Response */
void TATUDevice::generateBody(char *payload, uint8_t length, void (*callback)(uint32_t, char*)){
    int aux = last_char; /* **ponteiro** auxiliar do vetor de mensagens */
    char response[20];

    // O POST é importante pois define se a mensagem deve ser ignorada ou não logo
    // o parse deve identificar se a mensagem é um post

    // Se encontrados erros no PARSE retorne NULL
    if(requisition->parse(payload, length);) strcpy_P(SAIDA_STR, null_body);

    /** Aqui estarão contidos os 3 CALLBACKS**/
    switch(requisition->cmd.OBJ.SYS){
        // Função do usuario
        switch TATU_ALIAS:
            callback(requisition->str_hash, response)
            break;
        // Funções do sistema
        switch TATU_SYSTEM:
            /** **/
            break;
        switch TATU_PIN:
            /** **/
            break;
    }

    // Se a requisição não é um GET
    if(requisition->cmd.OBJ.TYPE != TATU_GET){
        // Se não encontrados erros no PARSE porém temos um SET ou EDIT
        strcpy_P(SAIDA_STR, true_body);
        return;
    }

    // Se foi lido um GET então temos que preparar o retorno
    // INCLUIR NO TPI 
    /*
        Propriedades de sistema serão precedidas por $
        informações que começam com um numero são pinos
        TATU_SYSTEM
        TATU_ALIAS
        TATU_PIN
        essas 3 constantes vão identificar qual o tipo do parametro
    */

    // Se for um pino ou uma configuração de sistema então o retorno está pronto
    if(requisition->cmd.SYS != TATU_ALIAS) return;

    // Copia o começo do body após a virgula ("BODY":{)
    strcpy_P(SAIDA_STR, body_str);
    aux += 8;

    // Nessa versão ainda não contamos com o suporte a multiplos parametros então apenas copiamos
    // A primeira variavel do payload
    QUOTE;
    strcpy(SAIDA_STR, payload);
    QUOTE;
    COLON;

    // O valor dessa variavel se é um número o retorno é inteiro caso contrário o retorno
    // é uma string
    if(ISNUM(response[0])) strcpy(SAIDA_STR, response);
    else{
        QUOTE;
        strcpy(SAIDA_STR, response);
        QUOTE;
    }

    // Fecha o JSON e a STRING
    BRACE_RIGHT;
    BRACE_RIGHT;
    CLOSE_MSG;
}