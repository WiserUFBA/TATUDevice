#ifndef TATUDevice_h
#define TATUDevice_h
typedef struct flowUnit {
  unsigned long int collect_freq, publish_freq;
  void* vector; uint8_t iterator, size;
  void* publish_method;
  void* message;
  uint32_t att, flow;
  flowUnit* next;
  int lastTimeCollect, lastTimePub;
  uint8_t type;
  bool used;
} flowUnit, *flowList;

class FlowController{
public:
    flowList activity;


    FlowController();
    void buffer_alloc(uint8_t size, flowList unit);
    void flowbuilder(char* json, uint32_t hash, uint8_t code);
    void* vector_iterator(flowList unit);
    void flow_loop(flowList unit);
    void requisition(void* response, uint32_t hash);
    void* vector_acess(flowList unit, int i);
    void push_value(char* response, flowList unit, int i);
    void flow_publish(flowList unit);
    void flow_construct(uint32_t hash, int collect_freq, void* message, int publish_freq, uint8_t code, uint8_t type, void* vector, uint32_t flow, flowList unit);

private:
    //flowList unit;
};
#endif