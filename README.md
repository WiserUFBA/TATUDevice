# TATUDevice
Biblioteca para a implementação do sistema operacional dos dispositvos TATU
--
[![Build Status](https://magnum.travis-ci.com/WiserUFBA/TATUDevice.svg?token=9bNq5p5MKERJTo9DstR2)](https://magnum.travis-ci.com/WiserUFBA/TATUDevice)

### Introdução
A biblioteca TATUDevice tem como o objetivo a implementação do sitema operacional TOS(Thing Operating System), sistema operacional criado, para facilitar e padroniza o desenvolvimento de dispositvos IoT da plataforma TATU.A Plataforma TATU ultiliza o protocolo TPI(Thing Protocol for Internet), um protocolo para comunicação entre dispositvos e foi desenvolvida pela equipe de hardware do projeto [SmartUFBA](http://wiki.dcc.ufba.br/SmartUFBA). 

A biblioteca TATUDevice usa o TPI sobre o protocolo [MQTT](http://mqtt.org/faq), um protocolo leve, popular para o uso na Internet das Coisas. Diferentemente do HTTP o MQTT não é baseado no paradigma requisição ↔ resposta, ao invés disso temos uma analogia parecida a um blog com um feed RSS ou uma revista com assinatura aonde as pessoas subscritas,  ou seja os assinantes recebem as publicações assim que os publicantes, publicam a informação. Este modelo é conhecido como modelo de publicação↑  subscrição↓.

Os tópicos são as vias de comunicação do protocolo e funcionam como um terminal de replicação, tudo que chega num determinado tópico é replicado para todos os clientes inscritos neste determinado tópico. O responsável por replicar estas mensagens é o “broker”.

### Quais as funcionalidades desse Sistema Operacional?

O TATUDevice permite uma ágil implementação de um dispositvo que use o protocolo TPI, além de providenciar uma 

interface para gereneciamento das configurações do dispositivo , como atributos que o caracterizem(e.g nome,id e ip do dispositivo) e para gerenciamento da comunicação do controlador com seus elementos(e.g sensores e atuadores).

### Como o TATU integra os elementos de um sistema IoT?

Através de um [Driver](https://github.com/WiserUFBA/DriverMQTT/blob/master/README.md) desenvolvido pelo projeto, que trata as mensagens enviadas pelos dispositivos e intermedia a comunicação destes com possíveis serviços que se utilizem de suas funcionalidades.

## The protocol

### The requisitions
	<type> 	: The type of the expected response(ex: int,str,bool...)
	<var> 	: The name of the sensor/actuactor/etc that is requested(ex: temperature, lamp, AirConditioner)
	<value>	: The value of the request(ex: on, off, 26, {"collect":800,"publish":"4000"}-read flow section-)
#### FLOW
FLOW \<type\> \<var\> \<value\>


	<example>
	FLOW INT temperatureSensor {"collect":500,"publish":2500}(-read flow section for more details-)
	</example>
#### GET
GET \<type\> \<var\> \<value\>


	<example>
	GET INFO lamp
	</example>
#### SET
SET \<type\> \<var\> \<value\>


	<example>
	SET BOOL lamp true
	</example>
###	The responses
	The response are always jsons with the above format:
	{"code":"post","HEADER":{"requisition":"<requisition>","name":"<deviceName>"},"BODY":{"<var>":<value>,"<aditionional>":"<aditionional>"...}}
#### Flow response
	<example>
	{"code":"post","HEADER":{"requisition":"FLOW","name":"device01"},"BODY":{"temperatureSensor":true}
	</example>
#### Get response
	<example>
	{"code":"post","HEADER":{"requisition":"GET","name":"device01"},"BODY":{"lamp":"off"}
	</example>	
#### Set response
	<example>
	{"code":"post","HEADER":{"requisition":"SET","name":"device01"},"BODY":{"lamp":true}
	</example>

## The FLOW feature
When a device receive a flow request it means that some client expects that the device delivers, with the requested frequence, the value of the last "n" values of a sensor, where "n" is the delivery time divided by the collect time.

### Example
Let's say that a client make that request for temperatureSensor's values:
FLOW INT temperatureSensor {"collect":500,"publish":2500}

It means that the device must collect data from it's temperatureSensor every 500ms(milliseconds) and deliver all that collected data(in form of a integer array) every 2500ms.

**That's how the delivered message is going look like:**

{"code":"post","HEADER":{"requisition":"FLOW","name":"device01"},"BODY":{"temperatureSensor":[27,28,27,27,27],flow:{"collect":500,"publish":2500}}



## TATUDevice

O instanciamento `device(...params...)` significa a criação de um dispositivo capaz de ler mensagens TPI e dar uma resposta adequada providenciada pelo desenvolvedor.

### Funcionalidades

- Criação automática de uma configuração para o dispositvo 
- Interface para gerenciamento das configurações
- Interface para comunicação com sensores e atuadores
- Sistema de comunicação externa através do protocolo TPI

### Baixando as classes

Você pode clonar esse repositório e adicionar suas pastas ao diretório de bibliotecas do Arduino, e simplesmente inclui-las(e.g. #include<TATUDevice.h>). é importante apontar que você precisa do TATUIntepreter para usar o TATUDevice.

## Configurando e usando o TATUDevice

Para o funcionamento correto, um dispositivo possui certos requisitos:

-Um cliente que lhe forneça conexão com a internet
-Um cliente que lhe forneça conexão via MQTT
-Um broker MQTT com o qual se comunicar
