////Thermisistors/////////////
//Use ESP32
//Original source: https://www.ametherm.com/blog/thermistor/arduino-and-thermistors
//Modified by Abd Almonem Doolaanea maker4ir@gmail.com

/*
This is the Apps SCript code, copy it and paste in the Apps Script:

-------------------------------------------
function doGet(e) {

    let id = '1v1aR-r9jwVDWpSDVHtgvjwqooD4D_LMNGCTnB5QFY-0'; 
    let sheetName = 'T_Log'; 
    var result;

    // e.parameter has received GET parameters, i.e. value0, value1, value3
    if (e.parameter == undefined) {
        result = 'Parameter undefined';
    } else {
        var sheet = SpreadsheetApp.openById(id).getSheetByName(sheetName);
        var newRow = sheet.getLastRow() + 1;  // get row number to be inserted
        var rowData = [];

        // get current time
        var formattedDate = Utilities.formatDate(new Date(), "GMT+8", "yyyy-MM-dd'T'HH:mm:ss");
        Logger.log(formattedDate);
        rowData[0] = formattedDate;
        rowData[1] = e.parameter.temperature;    
        rowData[2] = e.parameter.voltage;   


        // 1 x rowData.length cells from (newRow, 1) cell are specified
        var newRange = sheet.getRange(newRow, 1, 1, rowData.length);

        // insert data to the target cells
        newRange.setValues([rowData]);
        result =  'Data successfully inserted';
    }

    return ContentService.createTextOutput(result);
}
-----------------------------------------
*/

//////////Thermistor varibles///////////////
#define Thermistor_pin 35
int Vo = 0;
long V_total = 0;
int V_aver = 0;
#define R1  100000 //this is the fixed resitor value
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float temp;
////////////////////////////////

/////SD////////
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

//////////////////////////////////////

/////// Gsheet///////
/////// Wifi Settings & Gsheet///////
char ssid[] = "Batoul";
char pass[] = "12345678";

#include <HTTPClient.h>
HTTPClient http;
WiFiClient wifi;
int status = WL_IDLE_STATUS;


String serverAddress = "https://script.google.com/macros/s/AKfycbzZ59N_WOJZOoesyFI6vn5rPcuoW8IYLrDtf2Cj4_g2ME2Yd_hbgrSu5bKuCtmeMb-ReQ/exec?";  // server address

long timer0 = 0; //blynk timer
String postData = "";
/////////////////////////////

void setup() {
Serial.begin(115200);
//T
Serial.println("Temperature program started using thermistor and DS18B20");


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

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));


appendFile(SD, "/TempLog.txt", "Time,Temp");
appendFile(SD, "/TempLog.txt", "\n");

//WIFI//
WiFi.begin(ssid, pass);
Serial.println("Connecting");
while(WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.print("Connected to WiFi with IP Address: ");
Serial.println(WiFi.localIP());
/////////////////////

}

void loop() {
//measuring temperature from the thermistor
for (int i = 0; i<100; i++)
{
  Vo = analogRead(Thermistor_pin);
  V_total += Vo ;
  delay(10);
}
V_aver = V_total / 100 ;
//Serial.print("V_aver = "); Serial.println(V_aver); 
V_total = 0;

R2 = R1 * (4095.0 / (float)V_aver - 1.0);
logR2 = log(R2);
T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
temp = - (T - 273.15);

//Serial.println(R2);
Serial.print("Temp Thermistor = "); 
Serial.print(temp);
Serial.println(" C");

//SD
Temp_data = String(millis()) + "," + String(temp) + "\n";
appendFile(SD, "/TempLog.txt", Temp_data.c_str());

//send to gsheet
Serial.println("making GET request");
postData = serverAddress + "temperature=" + String(temp) + "&voltage=" + String(V_aver);
Serial.println(postData);
http.begin(postData);

int httpCode = http.GET();
if(httpCode > 0) {  
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      if(httpCode == HTTP_CODE_OK) {  
        String payload = http.getString();
        Serial.println(payload); 
      }
    }else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
http.end();
///////////

delay(5000);

}