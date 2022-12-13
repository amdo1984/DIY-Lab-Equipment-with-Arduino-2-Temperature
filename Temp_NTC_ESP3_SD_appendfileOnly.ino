////Thermisistors/////////////
//Use ESP32
//Original source: https://www.ametherm.com/blog/thermistor/arduino-and-thermistors
//Modified by Abd Almonem Doolaanea maker4ir@gmail.com

//////////Thermistor varibles///////////////
#define Thermistor_pin 39
int Vo = 0;
long V_total = 0;
int V_aver = 0;
#define R1  100000 //this is the fixed resitor value
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float temp;
////////////////////////////////

/////SD////////
//Using Appendfile function only.
String Temp_data = "" ;

#include "FS.h"
#include "SD.h"
#include "SPI.h"

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}


void setup() {
Serial.begin(115200);
//T
Serial.println("Temperature program started");

//SD
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    appendFile(SD, "/hello.txt", "World!\n");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));


appendFile(SD, "/TempLog.txt", "Time,Temp");
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

R2 = R1 * (4095.0 / (float)V_aver - 1.0);
logR2 = log(R2);
T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
temp = - (T - 273.15);

//Serial.println(R2);
Serial.print("Temp = "); Serial.print(temp); Serial.println(" C");

//SD
Temp_data = String(millis()) + "," + String(temp) + "\n";
appendFile(SD, "/TempLog.txt", Temp_data.c_str());
}