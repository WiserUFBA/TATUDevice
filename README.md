# TATUDevice
Biblioteca para a implementação do sistema operacional dos dispositvos TATU
--
[![Build Status](https://magnum.travis-ci.com/WiserUFBA/TATUDevice.svg?token=9bNq5p5MKERJTo9DstR2)](https://magnum.travis-ci.com/WiserUFBA/TATUDevice)

### Introdução
A biblioteca TATUDevice tem como o objetivo a implementação do sitema operacional TOS(Thing Operating System), sistema operacional criado, para facilitar e padroniza o desenvolvimento de dispositvos IoT da plataforma TATU.A Plataforma TATU ultiliza o protocolo TPI(Thing Protocol for Internet), um protocolo para comunicação entre dispositvos e foi desenvolvida pela equipe de hardware do projeto [SmartUFBA](http://wiki.dcc.ufba.br/SmartUFBA). 

A biblioteca TATUDevice usa o TPI sobre o protocolo [MQTT](http://mqtt.org/faq), um protocolo leve, popular para o uso na Internet das Coisas. Diferentemente do HTTP o MQTT não é baseado no paradigma requisição ↔ resposta, ao invés disso temos uma analogia parecida a um blog com um feed RSS ou uma revista com assinatura aonde as pessoas subscritas,  ou seja os assinantes recebem as publicações assim que os publicantes, publicam a informação. Este modelo é conhecido como modelo de publicação↑  subscrição↓.

Os tópicos são as vias de comunicação do protocolo e funcionam como um terminal de replicação, tudo que chega num determinado tópico é replicado para todos os clientes inscritos neste determinado tópico. O responsável por replicar estas mensagens é o “broker”.

###Quais as funcionalidades desse Sistema Operacional?

O TATUDevice permite uma ágil implementação de um dispositvo que use o protocolo TPI, além de providenciar uma 

interface para gereneciamento das configurações do dispositivo , como atributos que o caracterizem(e.g nome,id e ip do dispositivo) e para gerenciamento da comunicação do controlador com seus elementos(e.g sensores e atuadores).

###Como o TATU integra os elementos de um sistema IoT?

Através de um [Driver](https://github.com/WiserUFBA/DriverMQTT/blob/master/README.md) desenvolvido pelo projeto, que trata as mensagens enviadas pelos dispositivos e intermedia a comunicação destes com possíveis serviços que se utilizem de suas funcionalidades.

##TATUDevice

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

Até agora foram testadas as seguintes bibliotecas com o nosso framework.

-[SFE_CC3000_Library](https://github.com/sparkfun/SFE_CC3000_Library) para o uso com o shield wifi -[CC3000](https://www.sparkfun.com/products/12071) 
-[Adafruit_CC3000_Library](https://github.com/adafruit/Adafruit_CC3000_Library)
-[Ethernet]https://github.com/arduino/Arduino/tree/master/libraries/Ethernet

O device é composto por atributos que consideramos importantes para sua identificação, um objeto do tipo TATUInterpreter, que serve para analisar a mensagem recebida e preparar o dispositivo para uma resposta apropriada, o identificador do broker, a porta da comunicação com o broker, e o Callback, que é uma função á ser executada quando o cliente recebe uma mensagem vinda do tópico em que estiver inscrito.


```cpp
#include <TATUDevice.h>
#include <TATUInterpreter.h>

//declaração de macros e variáveis
#include MQTTPORT 1883
#include SYS_VERSION
//...
byte server_ip[] = { 192, 168, 0, 103 };
byte ip[4]    = { 192, 168, 0, 68};
//...

//Construção do device 
TATUInterpreter interpreter;
TATUDevice device(name, ip, pan_id, id, sample_rate, server_ip, MQTTPORT,
                  SYS_VERSION, &interpreter, callback);

// Funçao do usuario para variaveis do TATU
bool callback(uint32_t hash,char* response,char* valor,uint8_t type) {
  // Faça as comparações aqui
  
  //As comparações com o type são feitas para determinar que tipo de ação se espera 
  //TATU_SET serve para alterções no dispositivo TATU_GET para retorna informações requisitadas
  
  // As comparações realizadas com um hash DJB são feitas para se determinar
  // qual o atributo do dispositivo será usado e pode receber quantas 
  // clausulas você quiser 
  // o padrão deve ser return true mas pode ser substiuido por uma resposta true ou false
  
  switch(type){  
    case TATU_GET:
      switch(hash){
          break;  
      }
      break;
    case TATU_SET:
      switch(hash){
          break;  
      } 
      break;
  } 
  return true;
}

//O uso dessas macros é essencial para a correta interação do device com o cliente mqtt
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , wifi_client);
MQTT_PUBLISH(bridge, client);

void setup(){
  Serial.begin(BAUDRATE);
  
  //configurações de conexão e outras necessárias ao seu dispositivo
  //...
  //...
  
  //O cliente deve ser conectado no tópico relativo ao seu nome
  if(client.connect(device.name)){
    client.subscribe(device.name);
  }
  
}
```

### O Callback
O callback é a função chamada em resposta ao recebimento de uma mensagem, é ele que o programador usará para atender às requisições externas. Ele foi construído de forma evitar o máximo de erros possíveis, e efetuar as respostas de forma rapida e eficiciente.

#### Parâmetros
- hash: O hash é um númerio inteiro que representa o atributo à ser usado no callback
- response: Essé o parâmetro à ser utilizado pelo usuário para devolver à resposta em caso de uma requsição GET
- value: Nessa variável estar o valor a ser usado para a realização da alteração pedida em uma requisição SET
- type: é com essa variável que o a função determina o tipo de ação correspondente à requisição
```cpp
bool Callback(uint32_t hash,char* response,char* value,uint8_t type) 

```
#### Switch(type)

É atravéz do switch(type) que a função executa ação correta.

- TATU_GET: significa que houve uma requisição externa do valor de um atributo do dispositivo.
```cpp
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_lamp:
          strcpy(response,"resposta");
          break;

      }
      break;
```
- TATU_SET: significa houve uma requisição externa de alteração do valor de um atributo do dispositivo.
```cpp
    case TATU_SET:
      switch(hash){
        case H_lamp:
          strcpy(valor,value);
          break;
      } 
      break;
  } 
```
#### Switch(hash)

É atravéz do switch(hash) que a função executa ação sobre o atributo correto.
```cpp
bool callback(uint32_t hash,char* response,char* valor,uint8_t type) {
  
  switch(type){
    //caso seja um GET 
    case TATU_GET:
      switch(hash){
        //caso seja o atributo "lamp"
        case H_lamp:
          if(lamp) strcpy(response,"ON");//response tem valor "ON" se a lâmpada estiver ligada
          else strcpy(response,"OFF");//response tem valor "OFF" se a lâmpada estiver desligada
          break;
        //caso seja o atributo "valve"
        case H_valve:
          if(valve) strcpy(response,"OPEN");//response tem valor "OPEN" se a válvula estiver aberta
          else strcpy(response,"CLOSE");//response tem valor "CLOSE" se a válvula estiver aberta
          break;
        default:
          return false;
      }
      break;
    //caso seja um SET
    case TATU_SET:
      switch(hash){
        case H_lamp:
          if (valor[0] == 'D'){ligar(LAMP); lamp = true;}
          //Liga a lâmpada se o primeiro character do valor for 'L'
          else if (valor[0] == 'L'){desligar(LAMP); lamp = false;}
          //Desiga a lâmpada se o primeiro character do valor                                                                   //  for 'D'
          break;
        case H_valve:
          if (valor[0] == 'D'){ligar(VALVE); valve = true;}
          //Abre a válvula se o primeiro character do valor for 'A'
          else if (valor[0] == 'L'){desligar(VALVE); valve = false;}
          //Fecha a válvula se o primeiro character do valor                                                                    //for 'F'
          break;
          default:
            return false;
      } 
      break;
  } 
    return true;

}
```
#### Hash
  Hash como já dito é, um inteiro usado pelo device que representa o atributo relacionado à requisição. Como pode ser observado na definição do [TPI](https://github.com/WiserUFBA/TATU/blob/master/doc/tpi.txt) as requisições são do formato **"GET \<propriedade> \<var>"** em caso de requisições GET e **"SET \<propriedade> \<var> \<valor>"**. O hash representa o elemento \<var>, ele é conseguido através da aplicação de um algoritmo que converte-o para um hash djb.
  
A melhor maneira de se fazer isso é definindo um constante para representar o atributo.
Exemplo:

```cpp
#define H_lamp 2090464143 // representa a string "lamp"

bool callback(uint32_t hash,char* response,char* valor,uint8_t type) {
  
  switch(type){
    case TATU_GET:
      switch(hash){
        case H_lamp:
          if(lamp) strcpy(response,"ON");
          //response tem valor "ON" se a lâmpada estiver ligada
          else strcpy(response,"OFF");
          //response tem valor "OFF" se a lâmpada estiver desligada
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        case H_lamp:
          if (valor[0] == 'D'){ligar(LAMP); lamp = true;}
          else if (valor[0] == 'L'){desligar(LAMP); lamp = false;}
          break;
          default:
            return false;
      } 
      break;
  } 
    return true;

}

```
Assim sendo o a mensagem "SET INFO lamp D", desligará a luz.

No diretório [TATUConfig](https://github.com/WiserUFBA/TATUDevice/tree/master/TATUConfig) existe um programa escrito em C que gera define's dos hash's djb das strings informadas, e um arquivo .h que recomendamos usar para trabalhos que envolvem um considerável número de hash's. 

#### Retorno

O retorno deve ser true se não houver erros na execução e false se tiver algum.
```cpp
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_lamp:
          strcpy(response,"resposta");
          break;
        default:
          return: false;
      }
      break;
    case TATU_SET:
      switch(hash){
        case H_lamp:
          strcpy(valor,value);
          break;
        default:
          return: false;
      } 
      break;
  } 
  return true;
```

### Considerações sobre os formatos escolhidos
  O TATUDevice foi criado de maneira á reduzir o envolvimento com detalhes relativos a garantia do padrão, e da forma mais aberta possível de forma a permitir versatilidade para a contrução de dispositivos e liberdade para possíveis expansões do protocolo.
  O hash foi pensado para garantir agilidade nas comparações, assim como muitas outras soluções, visto que se pretende usá-lo em dispositivos limitados, e em projetos que tenham, entre outros objetivos, a pretensão de se diminur custos como o de energia.    
