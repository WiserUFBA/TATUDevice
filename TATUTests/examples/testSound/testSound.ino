#define SOUND_PIN A14
#define THRESHOLD 500

bool switcher = false;
int soundReading;
unsigned long int time,prevTime;

void volume(){   
 //Serial.println("Testing...");
 soundReading=analogRead(SOUND_PIN);
 if(soundReading < THRESHOLD){
   prevTime = millis();
   if(!switcher){
    switcher=true;
    Serial.println("alto");
   }
 }
 time = millis();
 //Serial.println(time - prevTime);
 if( (time - prevTime) > 1000){
   if(switcher){
     switcher = false;
     Serial.println("baixo");
   }
 }
}

void setup() {
  Serial.begin(9600);
  prevTime = millis();
}

void loop() {
  volume();
  soundReading=analogRead(SOUND_PIN);
  Serial.println(soundReading);
  delay(50);
}
