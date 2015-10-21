int gas_amount;

char response[30];


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  gas_amount = analogRead(A0);
  gas_amount = map (gas_amount,0,1023,0,100);
  //itoa(gas_amount,response,10);
  Serial.println(gas_amount);
  delay(1000);       
}
