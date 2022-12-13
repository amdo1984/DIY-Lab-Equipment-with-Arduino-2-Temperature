////Temperature Measurement/////////////
//Use ESP32
//Modified by Abd Almonem Doolaanea maker4ir@gmail.com

//This code has thermistor reading, DS18B20 eading, SD card, DHT, Google Sheet and OLED, you can disable any of them or adjust the way it work.


//////////OLED////////
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>


U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  // All Boards without Reset of the Display
//////////////////

//////////Thermistor varibles///////////////
#define Thermistor_pin 35
int Vo = 0;
long V_total = 0;
int V_aver = 0;
#define R1 100000  //this is the fixed resitor value
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float temp;
////////////////////////////////
/////SD////////
String Temp_data = "";

#include "FS.h"
#include "SD.h"
#include "SPI.h"

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}


void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}



//////////////////////////////////////

/////// Gsheet///////
/////// Wifi Settings & Gsheet///////
char ssid[] = "Maker4IR@unifi";
char pass[] = "Maker_4_IR";

#include <HTTPClient.h>
HTTPClient http;
WiFiClient wifi;
int status = WL_IDLE_STATUS;


String serverAddress = "https://script.google.com/macros/s/AKfycbzZ59N_WOJZOoesyFI6vn5rPcuoW8IYLrDtf2Cj4_g2ME2Yd_hbgrSu5bKuCtmeMb-ReQ/exec?";  // server address

String postData = "";
/////////////////////////////

/////////DS18B20////////
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 16

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

float tempC = 0.0;
/////////////////////////////

//////DHT/////

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 13
#define DHTTYPE DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

float T_DHT;
float H_DHT;
///////////////////////


void setup() {
  Serial.begin(115200);
  //T
  Serial.println("Temperature program started using thermistor and DS18B20");


  //SD
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));


  appendFile(SD, "/TempLog.txt", "Time,Temp C Thermistor,Temp C DS18B20");
  appendFile(SD, "/TempLog.txt", "\n");

  //WIFI//
  // WiFi.begin(ssid, pass);
  // Serial.println("Connecting");
  // while(WiFi.status() != WL_CONNECTED) {
  // delay(500);
  // Serial.print(".");
  // }
  // Serial.println("");
  // Serial.print("Connected to WiFi with IP Address: ");
  // Serial.println(WiFi.localIP());
  /////////////////////

  //DS18B20
  sensors.begin();

  //OLED
  u8g2.begin();
  u8g2.clearBuffer();                       // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);       // choose a suitable font
  u8g2.drawStr(10, 10, "Temp Measureing");  // write something to the internal memory
  u8g2.drawStr(10, 20, "Started...");       // write something to the internal memory
  u8g2.sendBuffer();                        // transfer internal memory to the display
  delay(2000);

  //clear the OLD
  u8g2.clearBuffer();
  u8g2.sendBuffer();
  //////////

  //DHT
  dht.begin();
  sensor_t sensor;
}

void loop() {
  //measuring temperature from the thermistor
  for (int i = 0; i < 100; i++) {
    Vo = analogRead(Thermistor_pin);
    V_total += Vo;
    delay(10);
  }
  V_aver = V_total / 100;
  //Serial.print("V_aver = "); Serial.println(V_aver);
  V_total = 0;

  R2 = R1 * (4095.0 / (float)V_aver - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  temp = -(T - 273.15);

  //Serial.println(R2);
  Serial.print("Temp Thermistor = ");
  //Serial.print("25");
  //Serial.print("\t");
  Serial.print(temp);
  //Serial.print("\t");
  Serial.println(" C");

  //measurign T using DS18B20
  sensors.requestTemperatures();  // Send the command to get temperatures
  tempC = sensors.getTempCByIndex(0);
  Serial.print("Temp digital = ");
  Serial.print(tempC);
  //Serial.print("\t");
  //Serial.println("35");
  Serial.println(" C");

  //SD
  Temp_data = String(millis()) + "," + String(temp) + "," + String(tempC) + "\n";
  appendFile(SD, "/TempLog.txt", Temp_data.c_str());

  //send to gsheet
  // Serial.println("making GET request");
  // postData = serverAddress + "temperature=" + String(temp) + "&voltage=" + String(V_aver);
  // Serial.println(postData);
  // http.begin(postData);

  // int httpCode = http.GET();
  // if(httpCode > 0) {
  //       Serial.printf("[HTTP] GET... code: %d\n", httpCode);

  //       if(httpCode == HTTP_CODE_OK) {
  //         String payload = http.getString();
  //         Serial.println(payload);
  //       }
  //     }else {
  //       Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  //     }
  // http.end();
  ///////////

  //DHT
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  T_DHT = event.temperature;
  dht.humidity().getEvent(&event);
  H_DHT = event.relative_humidity;

  Serial.print("T from DHT: ");
  Serial.println(T_DHT);
  Serial.print("H from DHT: ");
  Serial.println(H_DHT);

  //OLED
  u8g2.clearBuffer();

  u8g2.drawStr(5, 10, "Therm Temp:");
  u8g2.setCursor(80, 10);
  u8g2.print(temp);
  u8g2.drawStr(110, 10, "C");

  u8g2.drawStr(5, 30, "Digit Temp:");
  u8g2.setCursor(80, 30);
  u8g2.print(tempC);
  u8g2.drawStr(110, 30, "C");

  u8g2.sendBuffer();
  //delay(5000);
}