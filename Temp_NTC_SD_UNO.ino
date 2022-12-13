////Thermisistors/////////////
//Use UNO
//Original source: https://www.ametherm.com/blog/thermistor/arduino-and-thermistors
//Modified by Abd Almonem Doolaanea maker4ir@gmail.com

//////////Thermistor varibles///////////////
#define Thermistor_pin A0
int Vo = 0;
long V_total = 0;
int V_aver = 0;
#define R1  100000 //this is the fixed resitor value
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float temp;
////////////////////////////////

//////SD////////////////
#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;
String dataString = "";
//////////////////////

void setup() {
Serial.begin(115200);
//T
Serial.println("Temperature program started");

//SD
 Serial.print("Initializing SD card...");
   // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  File dataFile = SD.open("TempLog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println("Time,Voltage,Temperature");
    dataFile.close();
    // print to the serial port too:
    Serial.println("Time,Voltage,Temperature");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

}

void loop() {
//measuring temperature
for (int i = 0; i<100; i++)
{
  Vo = analogRead(Thermistor_pin);
  V_total += Vo ;
  delay(10);
}
V_aver = V_total / 100 ;
Serial.print("V_aver = "); Serial.println(V_aver); 
V_total = 0;

R2 = R1 * (1023.0 / (float)V_aver - 1.0);
logR2 = log(R2);
T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
temp = - (T - 273.15);

//Serial.println(R2);
Serial.print("Temp = "); Serial.print(temp); Serial.println(" C");

//SD 
dataString = String(millis()/1000) + "," + String(V_aver) + "," + String(temp);

File dataFile = SD.open("TempLog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

}
