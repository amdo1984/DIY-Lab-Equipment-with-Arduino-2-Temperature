////Thermisistors/////////////
//Thermistor varibles
#define Thermistor_pin A0
int Vo = 1;
long V_total = 1;
int V_aver = 1;
#define R1  100000 //this is the fixed resitor value
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float temp;


void setup() {
Serial.begin(115200);
Serial.println("Temperature program started");
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
V_total = 0;

R2 = R1 * (1023.0 / (float)V_aver - 1.0);
logR2 = log(R2);
T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
temp = -(T - 273.15);

//Serial.println(R2);
Serial.print("Temp = "); Serial.print(temp); Serial.println(" C");

}
