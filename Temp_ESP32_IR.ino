////Themperature Measurement//////
//Non-contact IR temperature sensor
//Use ESP32
//Modified by Abd Almonem Doolaanea maker4ir@gmail.com

#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
float tempIR = 0.00;



void setup() {
Serial.begin(115200);
Serial.println("IR T sensor MLX90614 test");
//check the connection
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1); //if starting the sensor failed, ESP32 will enter infinite loop => comment this line if you want it to continue.
  };
    Serial.print("Emissivity = "); Serial.println(mlx.readEmissivity());
  Serial.println("================================================");

}

void loop() {
tempIR = mlx.readObjectTempC();
Serial.print("*C\tObject = "); Serial.print(tempIR); Serial.println("*C");
delay(500) ;

}
