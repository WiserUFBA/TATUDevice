#include <TATUInterpreter.h>
#include <TATUDevice.h>
#include <stdint.h>
#include <PubSubClient.h>
#include "Client.h"



#define MAX_SIZE_AUX 25


class TATUOS{
public:
	TATUInterpreter interpreter;
	TATUDevice device;
	Callback callback;
	PubSubClient mqtt_client;

	char aux[MAX_SIZE_AUX];

	TATUOS(const char *name_d,   byte *ip_d, const int id_d,    const int pan_d,
                        const int sample_d,   byte *ip_m, const int port_m,  const int os_v,
                        bool (*callback_con)(uint32_t, bool*, bool, uint8_t),
                        Client &client);
	bool connect();
	void mqtt_callback(char *topic, byte *payload, unsigned int length);

};