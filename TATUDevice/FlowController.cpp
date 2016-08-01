#include "FlowController.h"

FlowController::FlowController(TATUDevice* aux_device){
	device = aux_device;
}

void* FlowController::vector_iterator(FlowList unit) {
	return ((unit->vector) + (unit->type * unit->iterator++));
}

void FlowController::loop() {
	FlowList unit = activity;
	while (unit) {
	if (!unit->used) break;
	if ((millis() / unit->collect_freq) >= unit->lastTimeCollect) {
	  unit->lastTimeCollect++;
	  //requisition_int(&temp_vector[temp_size++],unit->att);
	  //requisition_int(&((int*)fluxo->vector)[temp_size++],unit->att);
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
	uint8_t code;
	code = TATU_GET;
	device->get_function(hash, response, code);
}
void* FlowController::vector_acess(FlowList unit, int i) {
	return ((unit->vector) + (unit->type * i));
}

//Push the value to the response buffer
void FlowController::push_value(char* response, FlowList unit, int i) {
	switch (unit->type) {
		case sizeof(int):
		  itoa((*(int*)vector_acess(unit, i)), response, 10);

		  // do something
		  break;
		/*case sizeof(char*):
		  // do something
		  break;*/
		case sizeof(uint8_t):
		  itoa((*(int*)vector_acess(unit, i)), response, 10);
		  // do something
		  break;
		default:
		  return;
		  // do something
	}

}

//Who sends
void FlowController::flow_publish(FlowList unit) {
	uint8_t i, aux;
	char* response = vector_response;

	/*if (unit->flow == H_flow)
	response = vector_response;
	else response = vector_response2;*/

	response[0] = '\0';

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
	//unit->used = true;
	ATMSerial.println("Construiu!");
}
void FlowController::buffer_alloc(uint8_t size, FlowList unit) {
	//Allocate space on buffer according to the number of samples
	unit->vector = flow_buffer.end;
	unit->size = size;
	flow_buffer.end = ((unit->vector) + (unit->type * unit->size));
	ATMSerial.println("Alocou!");
}

void FlowController::flowbuilder(char* json, uint32_t hash, uint8_t code) {

	const int BUFFER_SIZE = JSON_OBJECT_SIZE(3);//needed to determine jsonbuffer size(abstarct it)
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);

	ATMSerial.print("JSON: ");
	ATMSerial.println(json);
	ATMSerial.print("collect :"); ATMSerial.println((int)root["collect"]);
	ATMSerial.print("publish :"); ATMSerial.println((int)root["publish"]);
	ATMSerial.print("type :"); ATMSerial.println((int)root["type"]);

	void* vector;
	FlowList unit = activity;

	//if type is '0' stop the attribute flow
	if (root["type"] == 1) {
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
	uint8_t type;
	switch (code) {
		case TATU_CODE_INFO:
			type = sizeof(char*);
			// do something
			break;
		case TATU_CODE_VALUE:
		  	type = sizeof(int);
		  	// do something
		  	break;
		case TATU_CODE_STATE:
		  	type = sizeof(bool);
		  	break;
		default:
		  	break;
	}

	//construct the flow unit

	ATMSerial.println("Construindo!");
	flow_construct(hash, root["collect"], (void*)get_flow,
	             root["publish"], TATU_GET, type, vector, H_flow, unit);

	//Allocate space on buffer according to the number of samples
	uint8_t size = unit->publish_freq / unit->collect_freq;
	unit->size = size;
	buffer_alloc(size, unit);
}
