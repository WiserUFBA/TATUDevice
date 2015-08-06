#define PRES 20
int count = 0;
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(PRES, INPUT);
  digitalWrite(PRES, HIGH);
  attachInterrupt(3, mexeu, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("mexeu");
  Serial.println(count);
  delay(1000);
}

void mexeu()
{
  count++;
  //iTime = millis();

}
