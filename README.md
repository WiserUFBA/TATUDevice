# TATUDevice
Biblioteca para a implementação do sistema operacional dos dispositvos TATU
--
### Introdução
A biblioteca TATUDevice tem como o objetivo a implementação do sitema operacional TOS(Thing Operating System), sistema operacional criado, para facilitar e padroniza o desenvolvimento de dispositvos IoT da plataforma TATU.A Plataforma TATU ultiliza o protocolo TPI(Thing Protocol for Internet), uma extensão do protocolo [MQTT](http://mqtt.org/faq) para comunicação entre dispositvos e foi desenvolvida pela equipe de hardware do projeto [SmartUFBA](http://wiki.dcc.ufba.br/SmartUFBA). 

###Quais as funcionalidades desse Sistema Operacional?
O TATUDevice permite uma ágil implementação de um dispositvo que use o protocolo TPI, além de providenciar uma interface para gereneciamento das configurações do dispositivo , como atributos que o caracterizem(e.g nome,id e ip do dispositivo) e para gerenciamento da comunicação do controlador com seus elementos(e.g sensores e atuadores).

###Como o TATU integra os elementos de um sistema IoT?
Através de um [Driver](https://github.com/WiserUFBA/DriverMQTT/blob/master/README.md) desenvolvido pelo projeto que trata as mensagens enviadas pelos e intermedia a comunicação desses dispositvos com possíveis serviços que se utilizem de suas funcionalidades,

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
//a completar




