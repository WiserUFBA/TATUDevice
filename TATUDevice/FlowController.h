#ifndef FlowController_h
#define FlowController_h

#include <stdint.h>

//#define flowList *FlowUnit


class FlowUnit {
  public:
    unsigned long int collect_freq, publish_freq;
    void* vector; uint8_t iterator, size;
    void* publish_method;
    void* message;
    uint32_t att, flow;
    FlowUnit* next;
    int lastTimeCollect, lastTimePub;
    uint8_t type;
    bool used;
};
typedef FlowUnit* FlowList;

class FlowController{
public:
    FlowUnit activity;

    FlowController();
    void buffer_alloc(uint8_t size, FlowList unit);
    void flowbuilder(char* json, uint32_t hash, uint8_t code);
    void* vector_iterator(FlowList unit);
    void flow_loop(FlowList unit);
    void requisition(void* response, uint32_t hash);
    void* vector_acess(FlowList unit, int i);
    void push_value(char* response, FlowList unit, int i);
    void flow_publish(FlowList unit);
    void flow_construct(uint32_t hash, int collect_freq, void* message, int publish_freq, uint8_t code, uint8_t type, void* vector, uint32_t flow, FlowList unit);

private:
    //FlowList unit;
};
#endif
