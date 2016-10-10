#ifndef FlowController_h
#define FlowController_h

#include <stdint.h>
#include <stdint.h>
#include "Arduino.h"
#include <TATUDevice.h>
#include <ArduinoJson.h>
//#include <Str.h>
//#define flowList *FlowUnit

#define DOD_T   0   
#define STR_T   1  
#define INT_T   2 
#define BOOL_T  3 

#define DEBUG
#define FLOW_DEBUG

//espITEAD
//#define ESP_F
#ifdef ESP_F
    #include <SoftwareSerial.h>
    #define DEBUG_PORT Serial
    // Debug Software Serial
    //SoftwareSerial static ESPSerial(12, 13);                 //Extra2 == 12 Extra3 == 13
#endif

#define PRINTLN(STR) DEBUG_PORT.println(STR)
#define PRINT(STR) DEBUG_PORT.print(STR)

#ifdef STANDARD
#define PRINTLN(STR) cout << STR << endl
#define PRINT(STR) cout << STR
#endif

#ifdef AVR_GCC
#define DOD_T   TATU_CODE_DOD   
#define STR_T   TATU_CODE_INFO  
#define INT_T   TATU_CODE_VALUE 
#define BOOL_T  TATU_CODE_STATE 

//#define FLOW_DEBUG 1

#define PRINTLN(STR) ATMSerial.println(STR)
#define PRINT(STR) ATMSerial.print(STR)
#endif

#define H_flow              0x7C96D85D
//const char get_flow[]     PROGMEM = "GET INFO flow";
const char get_flow[]     PROGMEM = "GET FLOW ";


class FlowUnit {
  public:
    unsigned long int collect_freq, publish_freq;
    void* vector; 
    void* vector_end; 
    int iterator;
    uint8_t size;
    uint8_t t_size;
    void* publish_method;
    char message[30];
    uint32_t att, flow;;
    FlowUnit* next;
    int lastTimeCollect, lastTimePub;
    uint8_t type;
    bool used;
    //char sensorName[25];
};
typedef FlowUnit* FlowList;

typedef struct sensorMap{
    uint32_t hash;
    char sensorName[25];
}sensorMap;

typedef struct flowBuffer {
  uint8_t vector[100];
  void* end;
} flowBuffer;

class FlowController{
public:
    FlowList activity;
    TATUDevice* device;
    char* vector_response;
    flowBuffer flow_buffer;
    sensorMap* sensors;
    //uint8_t flow_buffer[100];

    FlowController(TATUDevice* , char* );
    void buffer_alloc(FlowList unit);
    bool isInstantiated(FlowList unit,uint32_t hash);
    void flowbuilder(char* json, uint32_t hash, uint8_t code);
    void* vector_iterator(FlowList unit);
    void loop();
    void requisition(void* response, uint32_t hash,uint8_t code);
    void* vector_acess(FlowList unit, int i);
    void push_value(char* response, uint8_t type, void* iterator);
    void flow_publish(FlowList unit);
    void flow_pub(FlowList unit);
    void pubResponse(FlowList unit);

    void add_info(FlowList unit);

    void flowIteration(FlowList unit);
    void flow_construct(uint32_t hash, int collect_freq,const char* message, int publish_freq, uint8_t code, uint8_t type, void* vector, uint32_t flow, FlowList unit);
    void buildResponse(int* arr,int length);
    void buildResponse(char arr[][10],int length);//char matrix

    void* iterator_reset(FlowList unit);
    void push_method_char(char* vector,char* x);
    char* acess_method_char(char* vector,int length);
    int nextStr(char* str);

private:
    //FlowList unit;
};
#endif
