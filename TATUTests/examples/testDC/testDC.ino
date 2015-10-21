#define OPEN LOW
#define CLOSE HIGH

#define DIR_1 4
#define DIR_2 7
#define SPEED_PIN_1 5
#define SPEED_PIN_2 6

#define MOTORSPEED1 112
#define MOTORSPEED2 204
#define MOTORTIME1 1500
#define MOTORTIME2 1500
#define MOTORSTOP 0

bool persiana;
unsigned long currentMillis,previousMillis;

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
/*void fechar2(){
  digitalWrite(DIR_2,CLOSE);
  analogWrite(SPEED_PIN_2,MOTORSPEED2);
  delay(MOTORTIME2);
  analogWrite(SPEED_PIN_2,MOTORSTOP);
}*/
void setup() {
  // put your setup code here, to run once:
    char aux[16];  
  
  Serial.begin(9600);
  pinMode(DIR_1,OUTPUT);
  pinMode(DIR_2,OUTPUT);
  pinMode(SPEED_PIN_1,OUTPUT);
  pinMode(SPEED_PIN_2,OUTPUT); 
  previousMillis = 0;

}

void loop() {

  abrir1();abrir2();
  //Serial.println("Persiana aberta"); persiana = true;
  //delay(1000);
  for(previousMillis = millis(),currentMillis = millis();
      currentMillis - previousMillis < MOTORTIME1;
      currentMillis = millis());
  fechar1();fechar2();
  for(previousMillis = millis(),currentMillis = millis();
      currentMillis - previousMillis < MOTORTIME1;
      currentMillis = millis());
  //Serial.println("Persiana fechada"); persiana = false;
  delay(10000);
}
