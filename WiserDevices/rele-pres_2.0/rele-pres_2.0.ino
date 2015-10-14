#include <stdint.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Ethernet.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>

#define PRES1 3
#define PRES2 6
#define LAMP 8
#define GAS_PIN A0

#define MOVETIME 6000
#define MQTTPORT 1883

#define H_gas 193492480
#define H_lamp 2090464143
#define H_pres1 271186032
#define H_pres2 271186033

#define ligar(PIN) digitalWrite(PIN,true)
#define desligar(PIN) digitalWrite(PIN,false)

//variveis
bool lamp = 0,gas_amount,aux,presence = 0;
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xAC, 0xDC };
byte server[] = { 192, 168, 0, 101 };
byte ip[4]    = { 192, 168, 0, 127}; // Vetor nulo para que nao ocoram erros


int t,h,count1,count2;
unsigned long int time, lastConnect,prevTime,iTime;



// Funçao do usuario para variaveis do TATU
bool info(uint32_t hash,char* response,char* valor,uint8_t type) {
    
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_lamp:
          if(lamp) strcpy(response,"ON");
          else strcpy(response,"OFF");
          break;
        case H_gas:
          gas_amount = analogRead (GAS_PIN);
          gas_amount = map (gas_amount,0,1023,0,100);
          itoa(gas_amount,response,10);
          aux = strlen(response);
          response[aux++] = '%';
          response[aux] = 0;
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        case H_lamp:
          if (valor[0] == '1'){ligar(LAMP);Serial.println("ON"); lamp = true;}
          else if (valor[0] == '0'){desligar(LAMP);Serial.println("OFF"); lamp = false;}
          break;
        default:
          return false;
      } 
      //Serial.println(valor);
      break;
  } 
  return true;

}

//VALUE
bool value(uint32_t hash,uint16_t* response,uint16_t valor,uint8_t type){

  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_gas:
          *response = analogRead(GAS_PIN);
          *response = map (*response,0,1023,0,100);
          break;
        case H_pres1:
          *response = count1;
          count1 = 0;
          break;
        case H_pres2:
          *response = count2;
          count2 = 0;
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        default:
          return false;
      } 
      break;
  } 
  Serial.println(gas_amount);
  return true;
}
//STATE
bool state(uint32_t hash,bool* response,bool valor,uint8_t type){

  switch(type){  
    case TATU_GET:
      switch(hash){
          case H_lamp:
            *response = lamp;
            break;
          case H_gas:
            gas_amount = analogRead(GAS_PIN);
            gas_amount = map (gas_amount,0,1023,0,100);
            if (gas_amount > 55) *response = true;
            else *response = false;
            break;
          default:
            return false;
      }
      break;
    case TATU_SET:
      switch(hash){
          case H_lamp:
            if (valor){ligar(LAMP);Serial.println("ON"); lamp = true;}
            else {desligar(LAMP);Serial.println("OFF"); lamp = false;}
            break;
          default:
            return false;
      } 
      break;  
  }
  return true;
}

Callback callback = {
  .info = info,
  .value = value,
  .state = state
};

get_gas(uin8_t req){
;
  switch(req){
    case TATU
  }
            if (gas_amount > 55) *response = true;
            else *response = false;
}

bool get(uint32_t hash,void* response,uin8_t req){
  
  switch(hash){
      case H_lamp:
        *response = lamp;
        break;
      case H_gas:
        gas_amount = analogRead(GAS_PIN);
        gas_amount = map (gas_amount,0,1023,0,100)
        switch(req){   
          case TATU_CODE_INFO;
            itoa(gas_amount,response,10);
            aux = strlen(response);
            response[aux++] = '%';
            response[aux] = 0;
            break;
          case TATU_CODE_VALUE;
            *response = gas_amount;
            break;
          case TATU_CODE_STATE;
            if (gas_amount > 55) *response = true;
            else *response = false;
            break;
        } 
        break;
      default:
        return false;
  }

  
  return true;
  
}
bool set(uint32_t hash,void* response,uin8_t req){
    switch(hash){
      case H_lamp:
        *response = lamp;
        break;
      case H_gas:
        gas_amount = analogRead(GAS_PIN);
        gas_amount = map (gas_amount,0,1023,0,100)
        switch(req){   
          case TATU_CODE_INFO;
            itoa(gas_amount,response,10);
            aux = strlen(response);
            response[aux++] = '%';
            response[aux] = 0;
            break;
          case TATU_CODE_VALUE;
            *response = gas_amount;
            break;
          case TATU_CODE_STATE;
            if (gas_amount > 55) *response = true;
            else *response = false;
            break;
        } 
        break;
      default:
        return false;
  }

}

// Funçao do usuario para variaveis do TATU
bool info(uint32_t hash,char* response,char* valor,uint8_t type) {
    
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_lamp:
          if(lamp) strcpy(response,"ON");
          else strcpy(response,"OFF");
          break;
        case H_gas:
          gas_amount = analogRead (GAS_PIN);
          gas_amount = map (gas_amount,0,1023,0,100);
          itoa(gas_amount,response,10);
          aux = strlen(response);
          response[aux++] = '%';
          response[aux] = 0;
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        case H_lamp:
          if (valor[0] == '1'){ligar(LAMP);Serial.println("ON"); lamp = true;}
          else if (valor[0] == '0'){desligar(LAMP);Serial.println("OFF"); lamp = false;}
          break;
        default:
          return false;
      } 
      //Serial.println(valor);
      break;
  } 
  return true;

}

//VALUE
bool value(uint32_t hash,uint16_t* response,uint16_t valor,uint8_t type){

  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_gas:
          *response = analogRead(GAS_PIN);
          *response = map (*response,0,1023,0,100);
          break;
        case H_pres1:
          *response = count1;
          count1 = 0;
          break;
        case H_pres2:
          *response = count2;
          count2 = 0;
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        default:
          return false;
      } 
      break;
  } 
  Serial.println(gas_amount);
  return true;
}
//STATE
bool state(uint32_t hash,bool* response,bool valor,uint8_t type){

  switch(type){  
    case TATU_GET:
      switch(hash){
          case H_lamp:
            *response = lamp;
            break;
          case H_gas:
            gas_amount = analogRead(GAS_PIN);
            gas_amount = map (gas_amount,0,1023,0,100);
            if (gas_amount > 55) *response = true;
            else *response = false;
            break;
          default:
            return false;
      }
      break;
    case TATU_SET:
      switch(hash){
          case H_lamp:
            if (valor){ligar(LAMP);Serial.println("ON"); lamp = true;}
            else {desligar(LAMP);Serial.println("OFF"); lamp = false;}
            break;
          default:
            return false;
      } 
      break;  
  }
  return true;
}


// Objetos para exemplo usando interface internet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device("rele-pres", ip, 121, 88, 0, server, MQTTPORT, 1, &interpreter, callback);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
  char aux[16];  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  pinMode(LAMP,OUTPUT);
  
  pinMode(PRES1, INPUT);
  digitalWrite(PRES1, HIGH);
  //pinMode(PRES2, INPUT);
  //digitalWrite(PRES2, HIGH);
  
  attachInterrupt(1, mexeu1, FALLING);
  //attachInterrupt(0, mexeu2, FALLING);
 
  Serial.println("Done");
  
  //DEVICECONNECT(client,device);
  Serial.println("Tentando se conectar ao broker");  
  while(!client.connect(device.name,"device","boteco@wiser"));
  client.subscribe(device.name,1);
  Serial.println("Conectou ao broker");
  //wifi.getConnectionInfo(connection_info);
  //ipToString(connection_info.ip_address, aux);
  //Mdevice.generateHeader();
  //strcpy(device.ip, aux);
  lastConnect = millis();
  prevTime = millis();
  //lamp = digitalRead(RELE);
}
void loop() { client.loop(); 
  time = millis();
  if (time - lastConnect > 600000) {
    Serial.println("reconectando");
    client.disconnect();
    while(!client.connect(device.name,"device","boteco@wiser"));
    client.subscribe(device.name,1);
    lastConnect = millis();
  }

 
}
void mexeu1()
{
  count1++;
  //iTime = millis();
  Serial.println("mexeu1");
}
/*void mexeu2()
{
  count2++;
  //iTime = millis();
  Serial.println("mexeu2");
}*/
