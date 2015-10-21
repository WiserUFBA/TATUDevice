#define PHOTOPIN A15 // analog input
#define LEDPIN 10 // pin number for LED
int analogValue;

void setup() {
  // sets up pin input/output
  pinMode(LEDPIN, OUTPUT);
  pinMode(PHOTOPIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  // reads analog input
  analogValue = analogRead(PHOTOPIN);

  Serial.println(analogValue);
  
  // reads photoresistor every half second
  delay(500);
}
