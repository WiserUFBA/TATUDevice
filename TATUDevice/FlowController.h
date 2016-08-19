#ifndef FlowController_h
#define FlowController_h

#include <stdint.h>
#include <stdint.h>
#include "Arduino.h"
#include <TATUDevice.h>
#include <ArduinoJson.h>
//#define flowList *FlowUnit

#define DOD_T   0   
#define STR_T   1  
#define INT_T   2 
#define BOOL_T  3 

#ifdef STANDARD
#define PRINTLN(STR) cout << STR << endl
#define PRINT(STR) cout << STR
#endif

#ifdef AVR_GCC
#define DOD_T   TATU_CODE_DOD   
#define STR_T   TATU_CODE_INFO  
#define INT_T   TATU_CODE_VALUE 
#define BOOL_T  TATU_CODE_STATE 

#define PRINTLN(STR) ATMSerial.println(STR)
#define PRINT(STR) ATMSerial.print(STR)
#endif

#define H_flow              0x7C96D85D
const char get_flow[]     PROGMEM = "GET INFO flow";

class FlowUnit {
  public:
    unsigned long int collect_freq, publish_freq;
    void* vector; 
    void* iterator;
    uint8_t size;
    uint8_t t_size;
    void* publish_method;
    void* message;
    uint32_t att, flow;
    FlowUnit* next;
    int lastTimeCollect, lastTimePub;
    uint8_t type;
    bool used;
};
typedef FlowUnit* FlowList;

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
    //uint8_t flow_buffer[100];

    FlowController(TATUDevice* , char* );
    void buffer_alloc(FlowList unit);
    void flowbuilder(char* json, uint32_t hash, uint8_t code);
    void* vector_iterator(FlowList unit);
    void loop();
    void requisition(void* response, uint32_t hash);
    void* vector_acess(FlowList unit, int i);
    void push_value(char* response, FlowList unit, int i);
    void flow_publish(FlowList unit);
    void flow_construct(uint32_t hash, int collect_freq, void* message, int publish_freq, uint8_t code, uint8_t type, void* vector, uint32_t flow, FlowList unit);
    int nextStr(char* str);

private:
    //FlowList unit;
};
#endif
