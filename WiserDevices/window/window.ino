#include <stdint.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
#include <TATUDevice.h>
#include <TATUInterpreter.h>
#include <string.h>
#include <Servo.h>

#define H_persiana 1233304888
#define H_gas 193492480
#define H_rain 2090679631
#define H_ultrassonic 3496206460
#define H_move 2090515612
#define H_move1 267538509
#define H_move2 267538510
#define H_lamp 2090464143
#define H_light 266120797
#define H_servo 274290932
#define H_cam 193488118

#define MQTTPORT  2883
#define MQTTUSER  "device"
#define MQTTPASS  "boteco@wiser"

#define OPEN LOW
#define CLOSE HIGH

#define DIR_1 4
#define DIR_2 7
#define SPEED_PIN_1 5
#define SPEED_PIN_2 6
#define TRIB_PIN 8
#define ULTRASSONIC_PIN 9
#define LUMINOSITY A1
#define GAS_PIN 1
#define RAIN_PIN
#define MOVE1 20
#define MOVE2 21
#define LAMP 41
#define CAM 9

#define MOTORSPEED1 122
#define MOTORSPEED2 230
#define MOTORTIME1 1500
#define MOTORTIME2 1500
#define MOTORSTOP 0

#define ligar(PIN) digitalWrite(PIN,true)
#define desligar(PIN) digitalWrite(PIN,false)

bool persiana,lamp,raining;
unsigned long int currentMillis,previousMillis,lastConnect,time;
int gas_amount,count1,count2,pos = 0,new_pos =0;
Servo myservo;


// Funçao do usuario para variaveis do TATU  

bool info(uint32_t hash,char* response,char* valor,uint8_t type) {
  
  Serial.print("Hash :");
  Serial.println(hash);
  
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_persiana:
          if(persiana) strcpy(response,"OPEN");
          else strcpy(response,"CLOSE");
          break;
        case H_light:
          itoa(analogRead(LUMINOSITY),response,10);
          break;
        case H_gas:
          gas_amount = analogRead(GAS_PIN);
          gas_amount = map (gas_amount,0,1023,0,100);
          itoa(gas_amount,response,10);
          break;
        case H_ultrassonic:   
          break;
        case H_rain:
          if(raining) strcpy(response,"YES");
          else strcpy(response,"NO");
          break;
        case H_lamp:
          if (valor[0] == '1'){ligar(LAMP);Serial.println("ON"); lamp = true;}
          else if (valor[0] == '0'){desligar(LAMP);Serial.println("OFF"); lamp = false;}
          break;
        case H_move:
          itoa(count2,response,10);
          count1 = 0;
          break;
        case H_move1:
          itoa(count1,response,10);
          count1 = 0;
          break;
        case H_move2:
          itoa(count2,response,10);
          count2 = 0;  
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
        case H_persiana:
          if (valor[0] == 'T'){abrir1();abrir2();Serial.println("Persian aberta"); persiana = true;}
          else if (valor[0] == 'F'){fechar1();fechar2();Serial.println("Persiana fechada"); persiana = false;}
          break;
        case H_gas:
          break;
        case H_ultrassonic:
          break;
        case H_rain:
          break;    
        default:
          return false;
      } 
      break;
  } 
  
  Serial.println(valor);
  return true;

}
bool value(uint32_t hash,uint16_t* response,uint16_t valor,uint8_t type){
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_persiana:
          break;
        case H_light:
          *response = analogRead(LUMINOSITY);
          break;
        case H_gas:
          gas_amount = analogRead(GAS_PIN);
          gas_amount = map (gas_amount,0,1023,0,100);
          *response = gas_amount;
          break;
        case H_ultrassonic:
          break;
        case H_rain:
          break;
        case H_move:
          *response = count2;
          count2 = 0;
          break;
        case H_move1:
          *response = count1;
          count1 = 0;
          break;
        case H_move2:
          *response = count2;
          count2 = 0;
          break;
        case H_servo:
          *response = pos;
          break;
        default:
          return false;
      }
      break;
    case TATU_SET:
      switch(hash){
        case H_persiana:
          break;
        case H_gas:
          break;
        case H_ultrassonic:
          break;
        case H_cam:
          new_pos = valor;
          move();
          break;
        case H_rain:
          break;    
        default:
          return false;
      } 
      break;
  } 
  
  return true;
}
bool state(uint32_t hash,bool* response,bool valor,uint8_t type){
  switch(type){  
    case TATU_GET:
      switch(hash){
        case H_persiana:
          *response = valor;
          break;
        case H_gas:
          break;
        case H_ultrassonic:
          break;
        case H_rain:
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
        case H_persiana:
          if (valor){abrir1();abrir2();Serial.println("Persian aberta"); persiana = true;}
          else {fechar1();fechar2();Serial.println("Persiana fechada"); persiana = false;}
          break;
        case H_gas:
          break;
        case H_ultrassonic:
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

byte mac[]    = { 0xFA, 0xCA, 0xAA, 0x7F, 0x22, 0xCA, 0x9D, 0x5F };
byte server[] = { 10, 3, 200, 177 };
byte ip[]     = { 192, 168, 0, 165 };

// Objetos para exemplo usando interface internet
EthernetClient EthClient;
TATUInterpreter interpreter;
TATUDevice device("window", ip, 1, 1, 0, server, MQTTPORT, 1, &interpreter, callback);
MQTT_CALLBACK(bridge, device, mqtt_callback);
PubSubClient client(server, MQTTPORT, mqtt_callback , EthClient);
MQTT_PUBLISH(bridge, client);

/* Nao e necessario editar as linhas abaixo ao nao ser que tenha modificado alguma variavel */
void setup() {
  ConnectionInfo connection_info;
  char aux[16];
  Ethernet.begin(mac,ip);
  
  Serial.begin(9600);
  
  pinMode(LUMINOSITY,INPUT);
  pinMode(GAS_PIN,INPUT);
  
  pinMode(MOVE1, INPUT);
  digitalWrite(MOVE1, HIGH);
  pinMode(MOVE2, INPUT);
  digitalWrite(MOVE2, HIGH);
  
  attachInterrupt(2, mexeu2, FALLING);
  myservo.attach(CAM);


  
  /*Serial.println("Tentando conectar ao wifi");
  if ( !wifi.init() || !wifi.connect(ap_ssid, WLAN_SEC_WPA2, ap_password, TIMEOUT_CC3000)) Serial.println("Wifi nao conectado");
  else Serial.println("wifi conectado");
  Serial.println("Done");*/
    
  //DEVICECONNECT(client,device);
  Serial.println("Tentando se conectar ao broker");
  if(client.connect(device.name, MQTTUSER, MQTTPASS)){
    Serial.println("Conectou ao broker");
    client.subscribe(device.name);
  }
  else Serial.println("Nao conectou ao broker");
  //wifi.getConnectionInfo(connection_info);
  //ipToString(connection_info.ip_address, aux);
  //Mdevice.generateHeader();
  //strcpy(device.ip, aux);
  lastConnect = millis();
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
void move () {
  if (new_pos > pos)
    for(; pos <= new_pos ; pos += 1){ 
      myservo.write(pos);              
      delay(15); 
    }
  else
    for(; pos >= new_pos; pos -= 1) { 
      myservo.write(pos);              
      delay(15);
    } 
  
}

void mexeu1()
{
  count1++;
  //iTime = millis();
  Serial.println("mexeu1");
}
void mexeu2()
{
  count2++;
  //iTime = millis();
  Serial.println("mexeu2");
}
//Macro para abrir persiana1
void abrir1(){
  previousMillis = millis();
  digitalWrite(DIR_1,OPEN);
  analogWrite(SPEED_PIN_1,MOTORSPEED1);
  for(currentMillis = millis();
      currentMillis - previousMillis < MOTORTIME1;
      currentMillis = millis());
  Serial.println("Abriu 1");
  analogWrite(SPEED_PIN_1,MOTORSTOP);
}
//Macro para fechar persiana1
void fechar1(){
  previousMillis = millis();
  digitalWrite(DIR_1,CLOSE);
  analogWrite(SPEED_PIN_1,MOTORSPEED1);
  for(currentMillis = millis();
      currentMillis - previousMillis < MOTORTIME1;
      currentMillis = millis());  
  Serial.println("fechou 1");
  analogWrite(SPEED_PIN_1,MOTORSTOP);
}
//Macro para abrir persiana2
void abrir2(){
  previousMillis = millis();
  digitalWrite(DIR_2,OPEN);
  analogWrite(SPEED_PIN_2,MOTORSPEED2);
  for(currentMillis = millis();
      currentMillis - previousMillis < MOTORTIME1;
      currentMillis = millis());
  Serial.println("Abriu 2");
  analogWrite(SPEED_PIN_2,MOTORSTOP);
}
//Macro para fechar persiana2
void fechar2(){
  previousMillis = millis();
  digitalWrite(DIR_2,CLOSE);
  analogWrite(SPEED_PIN_2,MOTORSPEED2);
  for(currentMillis = millis();
      currentMillis - previousMillis < MOTORTIME1;
      currentMillis = millis());
  Serial.println("fechou 2");
  analogWrite(SPEED_PIN_2,MOTORSTOP);
}
