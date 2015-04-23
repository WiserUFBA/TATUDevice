#include <stdint.h>

TATUDevice::TATUDevice( const char *name_d, 	const char *ip_d, const uint8_t id_d, 	 const uint8_t pan_d,
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

TATUDevice::generateHeader(){
	// Primeiro se coloca a seguinte string padrão no vetor
}