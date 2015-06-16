#include "TATUOS.h"

const char DISO[]     PROGMEM = "DISO";
const char DOSI[]     PROGMEM = "DOSI";

TATUOS::TATUOS(const char *name_d, byte *ip_d, const int id_d, const int pan_d, const int sample_d,   
                        byte *ip_m, const int port_m,  const int os_v, 
                        bool (*callback_con)(uint32_t, bool*, bool, uint8_t), Client& client){
    //
	device = new TATUDevice(name_d,ip_d,id_d,pan_d,sample_d,ip_m,port_m,os_v,interpreter,callback_con);
	mqtt_client = new PubSubClient(server, mqtt_port, mqtt_callback , client,interpreter);

}

bool TATUOS::connect(){
	strcpy(aux,device.name);
	strcpy_P(aux[strlen(aux)],DISO);

	if(!client.connect(device.name))
	    return false;
	    
	client.subscribe(aux);
	return true;
}

void TATUOS::mqtt_callback(char *topic, byte *payload, unsigned int length){
    /* Gera o body e publica o mesmo */
    generateBody((char *) payload, (uint8_t) length);

    if(interpreter->cmd.OBJ.TYPE == TATU_POST){
        #ifdef DEBUG
        PRINT_DEBUG(IS_A_POST);
        DEBUG_NL;
        #endif
        return;
    }
    #ifdef DEBUG
    PRINT_DEBUG(PUBLISHING);
    Serial.println(device.output_message);
    #endif
    
    strcpy(aux,device.name);
	strcpy_P(aux[strlen(aux)],DOSI);
    //publica a mensagem
    mqtt_client.publish(aux, device.output_message);
    
    #ifdef DEBUG
    PRINT_DEBUG(PUBLISHED);
    DEBUG_NL;
    #endif

}


