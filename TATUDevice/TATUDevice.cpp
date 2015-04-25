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
const char false_body[]  PROGMEM = "\"BODY\":false}";
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
                        const uint8_t sample_d, const char *ip_m, const uint16_t port_m, const uint8_t os_v,
                        TATUInterpreter *req){
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
    requisition = req;

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
    COLON; BRACE_LEFT;
    
    // As próximas linhas produzem o HEADER
    strcpy_P(OUT_STR, header_str); /* Copia o HEADER */
    aux += 10;
    
    /* Coloca o NAME */
    strcpy_P(OUT_STR, name_str);
    aux += 8;
    strcpy(OUT_STR, device_name);
    aux += strlen(device_name);
    QUOTE; COMMA;

    /* Coloca o ID */
    strcpy_P(OUT_STR, id_str);
    aux += 5;
    itoa(device_id, aux_str, 10);
    strcpy(OUT_STR, aux_str);
    aux += strlen(aux_str);
    COMMA;
    
    /* Coloca o PAN */
    strcpy_P(OUT_STR, pan_str);
    aux += 6;
    itoa(device_pan, aux_str, 10);
    strcpy(OUT_STR, aux_str);
    aux += strlen(aux_str);
    COMMA;
    
    /* Coloca o IP */
    strcpy_P(OUT_STR, ip_str);
    aux += 6;
    strcpy(OUT_STR, device_ip);
    aux += strlen(device_ip);
    QUOTE;

    /* Fecha a mensagem de saída */
    BRACE_RIGHT; COMMA;
    CLOSE_MSG;

    last_char = aux;
}

void TATUDevice::generateBody(char *payload, uint8_t length, bool (*callback)(uint32_t, char*)){
    int aux = last_char;
    bool isString;
    char response[MAX_SIZE_RESPONSE] = 0;

    // Se encontrados erros no PARSE retorne "BODY":null
    if(requisition->parse(payload, length);){ strcpy_P(OUT_STR, null_body); return; }

    switch(requisition->cmd.OBJ.VAR){
        // Função do usuario
        switch TATU_TYPE_ALIAS:
            isString = callback(requisition->str_hash, response);
            break;
        // Funções do sistema
        switch TATU_TYPE_PIN:
            /** **/
            break;
        switch TATU_TYPE_SYSTEM:
            /** **/
            break;
    }

    if(requisition->cmd.OBJ.TYPE != TATU_GET){
        if(requisition->cmd.OBJ.ERROR) strcpy_P(OUT_STR, false_body);
        else strcpy_P(OUT_STR, true_body);
        return;
    }

    /* A função de edição dos pinos não necessita */
    if(requisition->cmd.OBJ.VAR != TATU_ALIAS) return;

    /* Coloca o BODY na resposta */
    strcpy_P(OUT_STR, body_str);
    aux += 8;

    // !IMPORTANT! Suporte para apenas uma variavel 
    /* Copia a variavel vinda do payload */
    QUOTE; strcpy(OUT_STR, payload); aux+=strlen(payload); QUOTE; COLON;

    /* Verifica se a resposta esta vazia ou se ela é um número ou bool para o JSON */
    if(ISEMPTY(response)) strcpy_P(OUT_STR, false_str);
    else if(isString) { strcpy(OUT_STR, response); aux+=strlen(response);}
    else{ QUOTE; strcpy(OUT_STR, response); aux+=strlen(response); QUOTE; }

    // Fecha o JSON e a STRING
    BRACE_RIGHT; BRACE_RIGHT;
    CLOSE_MSG;
}