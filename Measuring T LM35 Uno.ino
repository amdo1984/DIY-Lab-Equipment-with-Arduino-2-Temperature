////LM35/////////////
//Use UNO
//Original: https://lastminuteengineers.com/lm35-temperature-sensor-arduino-tutorial/
//Modified by Abd Almonem Doolaanea maker4ir@gmail.com

#define sensorPin A0

void setup() {
  // Begin serial communication 
  Serial.begin(115200);
  
}

void loop() {
  // Get the voltage reading from the LM35
  int reading = analogRead(sensorPin);
  Serial.print("reading: "); Serial.println(reading);

  // Convert that reading into voltage
  float voltage = reading * (5 / 1023);

  // Convert the voltage into the temperature in Celsius: Output scale factor	10mV/°C
  float temperatureC = voltage * 100; 

  // Print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.print("\xC2\xB0"); // shows degree symbol
  Serial.print("C  |  ");
  
  // Print the temperature in Fahrenheit
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print(temperatureF);
  Serial.print("\xC2\xB0"); // shows degree symbol
  Serial.println("F");

  delay(1000); // wait a second between readings
}
