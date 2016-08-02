#include "FlowController.h"
#define DOD_T 	TATU_CODE_DOD   
#define STR_T 	TATU_CODE_INFO  
#define INT_T	TATU_CODE_VALUE 
#define BOOL_T	TATU_CODE_STATE 

FlowController::FlowController(TATUDevice* aux_device, char* aux_response){
	device = aux_device;
	vector_response = aux_response;
	flow_buffer.end = flow_buffer.vector;
}

void* FlowController::vector_iterator(FlowList unit) {
	/*if (unit->type == STR_T){

	}
	else ;
	return unit->iterator;*/
	return (unit->vector) + (unit->type * unit->iterator++);
}

void FlowController::loop() {
	FlowList unit = activity;
	while (unit) {
		if (!unit->used) break;
		if ((millis() / unit->collect_freq) >= unit->lastTimeCollect) {
		  unit->lastTimeCollect++;
		  requisition(vector_iterator(unit), unit->att);
		}
		if ((millis() / unit->publish_freq) >= unit->lastTimePub) {
		  unit->lastTimePub++;
		  flow_publish(unit);
		  unit->iterator = 0;
	}
	unit = unit->next;
	}

}
//Who collects the samples(void*)
void FlowController::requisition(void* response, uint32_t hash) {
	ATMSerial.begin(115200);
	uint8_t code;
	code = TATU_GET;
	device->get_function(hash, response, code);
	ATMSerial.println(*(int*)response);
}
#define nextStr(STR,COUNT) while(STR[COUNT]++)
void* FlowController::vector_acess(FlowList unit, int i) {

	if (unit->type == sizeof(char*)){
		int j,k;
		char* str = (char*)unit->vector;
		for (j = 0; j < i; ++j){
			nextStr(str,k);
		}
	}

	return ((unit->vector) + (unit->type * i));
	
}

//Push the value to the response buffer
void FlowController::push_value(char* response, FlowList unit, int i) {
	switch (unit->type) {
		case STR_T:
			//strcpy((*(int*)vector_acess(unit, i)));
		  	itoa((*(int*)vector_acess(unit, i)), response, 10);
		  	// do something
		  break;
		case INT_T:
		  	itoa((*(int*)vector_acess(unit, i)), response, 10);
		  	// do something
		  	break;
		case BOOL_T:
		  	// do something
		  	break;
		default:
			itoa((*(int*)vector_acess(unit, i)), response, 10);
		  return;
		  // do something
	}
}

//Who sends
void FlowController::flow_publish(FlowList unit) {
	uint8_t i, aux;
	char* response = vector_response;

	response[0] = '\0';

	ATMSerial.print("Unit: ");
	ATMSerial.print("collect :"); ATMSerial.println(unit->collect_freq);
	ATMSerial.print("publish :"); ATMSerial.println(unit->publish_freq);

	ATMSerial.print("Size : ");
	ATMSerial.println(unit->size);
	for (i = 0; i < unit->size; i++) {
		aux = strlen(response);
		response[aux] = ',';
		response[++aux] = '\0';
		//Dinamic array builder!
		ATMSerial.print("Pushing : ");
		ATMSerial.println((*(int*)vector_acess(unit, i)));
		push_value(&response[aux], unit, i);
	}

	//encapsula nos colchetes
	response[0] = '[';
	aux = strlen(response);
	response[aux] = ']';
	response[++aux] = '\0';

	byte req[20];

	strcpy_P((char*)req, (const char*)unit->message);

	device->mqtt_callback("", req, strlen((char*)req) );
	ATMSerial.println(response);
}

void FlowController::flow_construct(uint32_t hash, int collect_freq, void* message, int publish_freq, uint8_t code, uint8_t type, void* vector, uint32_t flow, FlowList unit) {
	//This function constructs the flow unit
	ATMSerial.begin(115200);
	ATMSerial.println("Entrou!");
	unit->iterator = 0;
	unit->collect_freq = collect_freq;
	unit->publish_freq = publish_freq;
	unit->lastTimeCollect = millis() / collect_freq;
	unit->lastTimePub = millis() / publish_freq;
	unit->att = hash;
	unit->vector = vector;
	unit->type = type;
	unit->flow = flow;
	unit->message = message;
	unit->used = true;

	uint8_t size = unit->publish_freq / unit->collect_freq;
	unit->size = size;
	//unit->used = true;
	ATMSerial.println("Construiu!");
}
void FlowController::buffer_alloc(FlowList unit) {
	//Allocate space on buffer according to the number of samples
	unit->vector = flow_buffer.end;
	flow_buffer.end = ((unit->vector) + (unit->type * unit->size - 1));
	ATMSerial.println("Alocou!");

}

void FlowController::flowbuilder(char* json, uint32_t hash, uint8_t code) {

	ATMSerial.begin(115200);

	ATMSerial.println("Birll");
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(3);//needed to determine jsonbuffer size(abstarct it)
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);

	ATMSerial.print("JSON: ");
	ATMSerial.println(json);
	ATMSerial.print("collect :"); ATMSerial.println((int)root["collect"]);
	ATMSerial.print("publish :"); ATMSerial.println((int)root["publish"]);
	ATMSerial.print("turn :"); ATMSerial.println((int)root["turn"]);

	void* vector;
	FlowList unit = activity;

	//if turn is '0' stop the attribute flow
	if (root["turn"] == 1) {
		while (unit->att != hash) {
		  	unit = unit->next;
		  	// statement
		}
		unit->used = false;
		return;
	}

	ATMSerial.println("Checando");
	//Seek for unused flow unit
	while (unit->used) {
		unit = unit->next;
	}

	//set the type
	uint8_t type = sizeof(int);

	//construct the flow unit
	ATMSerial.println("Construindo!");
	vector = flow_buffer.end;
	flow_construct(hash, root["collect"], (void*)get_flow,
	             root["publish"], TATU_GET, type, vector, H_flow, unit);
	return;
	//Allocate space on buffer according to the number of samples
	buffer_alloc(unit);
}
