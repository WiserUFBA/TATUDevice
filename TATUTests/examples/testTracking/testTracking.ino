#define DOOR 6
///Arduino Sample Code
void setup()
{
  pinMode(DOOR,INPUT);
  Serial.begin(9600);
}
void loop()
{
  Serial.println(digitalRead(2)); // print the data from the sensor
  delay(500);
}
