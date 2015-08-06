#include <Ethernet.h>
#define DTEMP A13
int temp;
char str[20];
double temperature(){
  double Temp;
  Temp = log(10000.0*((1024.0/analogRead(DTEMP)-1)));
  //         =log(10000.0/(1024.0/RawADC-1)) // for pull-up configuration
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15 ;            // Convert Kelvin to Celcius
  //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
  return Temp ;
}
void setup() {
  Ethernet.begin();
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  temp = int (temperature());
  temp *= -1; 
  temp += 8;
  itoa(temp,str,10);
  Serial.println(str);
  delay(500);
}
