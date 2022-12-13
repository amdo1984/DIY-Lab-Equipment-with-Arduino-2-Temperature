////LM35/////////////
//Use ESP32
//Original: https://learn.adafruit.com/thermocouple/
//Modified by Abd Almonem Doolaanea maker4ir@gmail.com


#include "max6675.h"

int thermoDO = 19;
int thermoCS = 5;
int thermoCLK = 18;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

float T_thermocouple ;

void setup() {
  Serial.begin(9600);

  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {
  // basic readout test, just print the current temp
   T_thermocouple = thermocouple.readCelsius();
  
   Serial.print("C = "); 
   Serial.println(T_thermocouple);
  //  Serial.print("F = ");
  //  Serial.println(thermocouple.readFahrenheit());
 
   // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
   delay(1000);
}
