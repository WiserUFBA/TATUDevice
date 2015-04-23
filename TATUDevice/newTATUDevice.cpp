#include "TATUDevice.h"
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>

// Constantes
FLASH_DECLARE(char start_post = "POST ");
FLASH_DECLARE(char null_body = "\"BODY\":null}");
FLASH_DECLARE(char header_str = "\"HEADER\":{");
FLASH_DECLARE(char name_str = "\"NAME\":\"");
FLASH_DECLARE(char id_str = "\"ID\":");
FLASH_DECLARE(char pan_str = "\"PAN\":");
FLASH_DECLARE(char ip_str = "\"IP\":\"");
FLASH_DECLARE(char body_str = "\"BODY\":{");

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

TATUDevice::put_braces(char *brace_place, bool direction){
    *brace_place = direction ? '{' : '}'; 
}

TATUDevice::put_colon(char *colon_place, bool string){
    colon_place[0] = ':';
    if(string) colon_place[1] = '\"';
}

TATUDevice::put_comma(char *comma_place, bool string){
    if(string) comma_place[0] = '\"';
    colon_place[1] = ','
}

TATUDevice::put_colon_braces(char *brace_place){
    brace_place[0] = ':';
    brace_place[1] = '{'; 
}

TATUDevice::generateHeader(){
    /* Auxiliary variable */
    int aux;
    char aux_str[10];

    // Primeiro se coloca a seguinte string padrão no vetor
    strcpy_P(output_message, start_post);
    strcpy(&output_message[5], device_name);
    
    // Inicia o JSON
    aux = strlen(device_name) + 5;
    put_colon_braces(&output_message[aux]);
    aux += 2;
    
    // As próximas linhas produzem o HEADER
    strcpy_P(&output_message[aux], header_str); /* Copia o HEADER */
    aux += 10;
    
    /* Coloca o NAME */
    strcpy_P(&output_message[aux], name_str);
    aux += 8;
    strcpy(&output_message[aux], device_name);
    aux += strlen(device_name);
    put_comma(&output_message[aux], ISSTRING);
    aux += 2;

    /* Coloca o ID */
    strcpy_P(&output_message[aux], id_str);
    aux += 5;
    itoa(device_id, aux_str, 10);
    strcpy(&output_message[aux], aux_str);
    aux += strlen(aux_str);
    put_comma(&output_message[aux], ISNOTSTRING);
    aux += 1;
    
    /* Coloca o PAN */
    strcpy_P(&output_message[aux], pan_str);
    aux += 6;
    itoa(device_pan, aux_str, 10);
    strcpy(&output_message[aux], aux_str);
    aux += strlen(aux_str);
    put_comma(&output_message[aux], ISNOTSTRING);
    aux += 1;
    
    /* Coloca o IP */
    strcpy_P(&output_message[aux], ip_str);
    aux += 6;
    strcpy(&output_message[aux], device_ip);
    aux += strlen(device_ip);
    
    /* Fecha a mensagem de saída */
    output_message[aux++] = '\"';
    output_message[aux++] = '}';
    output_message[aux++] = ',';
    output_message[aux] = 0;

    last_char = aux;
}

TATUDevice::generateBody(bool success){
    if(!success) strcpy_P(&output_message[last_char], null_body);
    else{
        
    }
}

// 