#include "TATUDevice.h"

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
    int i,elemLength,length;
    char var[20];
    char value[20];
    header = "\"HEADER\":{";

    STRLEN(header,length);

    //cout << "1\n";    
    sprintf(var, "%s","ID");
    sprintf(value, "%d",DEVICE_ID);
    WRITE(header,var,value,elemLength,length);
    
  
    sprintf(var, "%s","NAME");
    sprintf(value, "%s",DEVICE_NAME);
    ELEMENTWRITE(header,var,value,elemLength,length);


    sprintf(var, "%s","IP");
    sprintf(value, "%s",DEVICE_IP);
    ELEMENTWRITE(header,var,value,elemLength,length);

    sprintf(var, "%s","PAN_ID");
    sprintf(value, "%d",PAN_ID);
    ELEMENTWRITE(header,var,value,elemLength,length);

    CONCATSINGLE(header,'}',length);

}
void Device::generatePost(uint32_t str_hash){
    sprintf(msg,"%s","POST <");
    bool retained = false;
    int i,j,length = 0,elemLength = 0;
    //char aux[100];
    char var[20];
    char value[20];

    //É preciso saber o tamanho da string para adicionar outra
    STRLEN(msg,length);
    CONCAT(msg,DEVICE_NAME,length);
    //Adiciona o header
    CONCAT(msg,">:{",length);
    CONCAT(msg,header,length);
    CONCATSINGLE(msg,",",length);
    //começa a adição do body
    CONCAT(msg,"\"BODY\":{",length);


    CONCAT(msg,header,length);

    switch(cmd.OBJ.CODE){
        case TATU_CODE_INFO:
            switch(info){
                default:
                    for (i = 0;i < PIN_AMOUNT && pinsWS_alias[i] != info; i++);
                        if (i < PIN_AMOUNT){
                            sprintf(value,"%d",pins_value[i]);
                            WRITE(msg,pins_alias[i],pins_value[i],elemLength,length);
                        }
                            //msg += info + ":" + pins_value[i];
                case H_PAN_ID:
                    sprintf(value,"%d",PAN_ID);
                    WRITE(msg,"\"PAN_ID\"",value,elemLength,length);
                    break;
                case RESET_TIMES:
                    sprintf(value,"%d",RESET_TIMES);
                    WRITE(msg,"\"RESET_TIMES\"",value,elemLength,length);
                    break;
                case H_START_TIME:
                    sprintf(value,"%d",START_TIME);
                    WRITE(msg,"\"START_TIME\"",value,elemLength,length);
                case H_SAMPLE_RATE:
                    sprintf(value,"%d",);
                    WRITE(msg,"\"SAMPLE_RATE\"",value,elemLength,length);
                    break;
                case H_OS_VERSION:
                    sprintf(value,"%d",OS_VERSION);
                    WRITE(msg,"\"OS_VERSION\"",value,elemLength,length);
                    break;
                case H_DEVICE_ID:
                    sprintf(value,"%d"DEVICE_ID,);
                    WRITE(msg,"\"DEVICE_ID\"",value,elemLength,length);
                    break;
                case H_DEVICE_NAME:
                    sprintf(value,"%d",DEVICE_NAME);
                    WRITE(msg,"\"DEVICE_NAME\"",value,elemLength,length);
                    break;

            }
            break;

        case TATU_CODE_STATE:
            sprintf(value,"%d",pins_value[cmd.OBJ.PIN]);
            WRITE(msg,"\"PIN\"",value,elemLength,length);
            break;

        case TATU_CODE_ALL: 

            for (i = 0,sprintf(value, "%d",pins_value[i]), WRITE(msg,pins_alias[i++],value,elemLength,length);i < PIN_AMOUNT; 
                ELEMENTWRITE(msg,pins_alias[i++],value,elemLength,length) );
            break;

    }

    //fecha o body
    CONCATSINGLE(msg,'}',length);
    //fecha o arquivo
    CONCATSINGLE(msg,'}',length);

}
