////Thermisistors/////////////
//Use UNO
//Original source: https://www.ametherm.com/blog/thermistor/arduino-and-thermistors
//Modified by Abd Almonem Doolaanea maker4ir@gmail.com


//////////Thermistor varibles///////////////
#define Thermistor_pin A0
int Vo = 0;
#define R1  100000 //this is the fixed resitor value
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float temp;
////////////////////////////////


void setup() {
Serial.begin(115200);
//T
Serial.println("Temperature program started");


}

void loop() {
//measuring temperature
Vo = analogRead(Thermistor_pin);
R2 = R1 * (1023.0 / (float)V_aver - 1.0);
logR2 = log(R2);
T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
temp = - (T - 273.15);

//Serial.println(R2);
Serial.print("Temp = "); Serial.print(temp); Serial.println(" C");

}
