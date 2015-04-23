#include "TATUInterpreter.h"

Device::Device(uint8_t pan, uint8_t reset, uint16_t start, uint8_t values[PIN_AMOUNT], uint8_t sample, 
    uint8_t os, uint8_t device_id, char alias[PIN_AMOUNT][6], char* name, char* IP, 
    char* server, char *topic){
    //
    int i = 0;

    PAN_ID = pan;
    RESET_TIMES = reset;
    START_TIME = start;
    for(i = 0; i < PIN_AMOUNT; i++)
        pins_value[i] = value[i];
    SAMPLE_RATE = sample;
    OS_VERSION = os;
    DEVICE_ID = device_id;
    for (int i = 0; i < count; ++i)
        sprintf(alias[i],"%s",pins_alias[i]);    
    sprintf(DEVICE_NAME,"%s",name);
    sprintf(DEVICE_IP,"%s",IP);
    sprintf(MQTT_SERVER,"%s",server);
    sprintf(postTopic,"%s",topic);
    cmd.STRUCTURE = 0;

    header = "\"HEADER\"";
    setHeader();
}

void Device::generateHeader(){
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


    CONCAT(msg,header);

    switch(cmd.OBJ.CODE){
        case TATU_CODE_INFO:
            switch(info){
                default:
                    for (i = 0;i < PIN_AMOUNT && pinsWS_alias[i] != info; i++);
                        if (i < PIN_AMOUNT){
                            sprintf(value,"%d",pins_value[i]);
                            WRITE(msg,pins_alias[i],pins_value[i],length);
                        }
                            //msg += info + ":" + pins_value[i];
                case H_PAN_ID:
                    sprintf(value,"%d",PAN_ID);
                    WRITE(msg,"\"PAN_ID\"",value,length);
                    break;
                case RESET_TIMES:
                    sprintf(value,"%d",RESET_TIMES);
                    WRITE(msg,"\"RESET_TIMES\"",value,length);
                    break;
                case H_START_TIME:
                    sprintf(value,"%d",START_TIME);
                    WRITE(msg,"\"START_TIME\"",value,length);
                case H_SAMPLE_RATE:
                    sprintf(value,"%d",);
                    WRITE(msg,"\"SAMPLE_RATE\"",value,length);
                    break;
                case H_OS_VERSION:
                    sprintf(value,"%d",OS_VERSION);
                    WRITE(msg,"\"OS_VERSION\"",value,length);
                    break;
                case H_DEVICE_ID:
                    sprintf(value,"%d"DEVICE_ID,);
                    WRITE(msg,"\"DEVICE_ID\"",value,length);
                    break;
                case H_DEVICE_NAME:
                    sprintf(value,"%d",DEVICE_NAME);
                    WRITE(msg,"\"DEVICE_NAME\"",value,length);
                    break;

            }
            break;

        case TATU_CODE_STATE:
            sprintf(value,"%d",pins_value[cmd.OBJ.PIN]);
            WRITE(msg,"\"PIN\"",value,length);
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
