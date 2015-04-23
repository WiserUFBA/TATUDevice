#include "TATUDevice.h"
#include "hash_list.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

// Constantes
const char start_post[] PROGMEM = "POST ";
const char null_body[]  PROGMEM = "\"BODY\":null}";
const char header_str[] PROGMEM = "\"HEADER\":{";
const char name_str[]   PROGMEM = "\"NAME\":\"";
const char id_str[]     PROGMEM = "\"ID\":";
const char pan_str[]    PROGMEM = "\"PAN\":";
const char ip_str[]     PROGMEM = "\"IP\":\"";
const char body_str[]   PROGMEM = "\"BODY\":{";

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

void TATUDevice::put_braces(char *brace_place, bool direction){
    *brace_place = direction ? '{' : '}'; 
}

void TATUDevice::put_colon(char *colon_place, bool string){
    colon_place[0] = ':';
    if(string) colon_place[1] = '\"';
}

void TATUDevice::put_comma(char *comma_place, bool string){
    if(string){ comma_place[0] = '\"'; comma_place[1] = ',';}
    else comma_place[0] = ',';
}

void TATUDevice::put_colon_braces(char *brace_place){
    brace_place[0] = ':';
    brace_place[1] = '{'; 
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
    put_colon_braces(SAIDA_STR);
    aux += 2;
    
    // As próximas linhas produzem o HEADER
    strcpy_P(SAIDA_STR, header_str); /* Copia o HEADER */
    aux += 10;
    
    /* Coloca o NAME */
    strcpy_P(SAIDA_STR, name_str);
    aux += 8;
    strcpy(SAIDA_STR, device_name);
    aux += strlen(device_name);
    put_comma(SAIDA_STR, ISSTRING);
    aux += 2;

    /* Coloca o ID */
    strcpy_P(SAIDA_STR, id_str);
    aux += 5;
    itoa(device_id, aux_str, 10);
    strcpy(SAIDA_STR, aux_str);
    aux += strlen(aux_str);
    put_comma(SAIDA_STR, ISNOTSTRING);
    aux += 1;
    
    /* Coloca o PAN */
    strcpy_P(SAIDA_STR, pan_str);
    aux += 6;
    itoa(device_pan, aux_str, 10);
    strcpy(SAIDA_STR, aux_str);
    aux += strlen(aux_str);
    put_comma(SAIDA_STR, ISNOTSTRING);
    aux += 1;
    
    /* Coloca o IP */
    strcpy_P(SAIDA_STR, ip_str);
    aux += 6;
    strcpy(SAIDA_STR, device_ip);
    aux += strlen(device_ip);
    
    /* Fecha a mensagem de saída */
    output_message[aux++] = '\"';
    output_message[aux++] = '}';
    output_message[aux++] = ',';
    output_message[aux] = 0;

    last_char = aux;
}

void TATUDevice::generateBody(bool success){
    if(!success) strcpy_P(&output_message[last_char], null_body);
    else{

    }
}

// 