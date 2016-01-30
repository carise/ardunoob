#include <math.h>

const int B = 4275;
const int R0 = 100000;
const int pinTempSensor = A2;
const int pinLED = 4;

const float TEMP_THRESHOLD = 59.9;

void setup() {
  Serial.begin(9600);
  pinMode(pinLED, OUTPUT);
}

void loop() {
  int val = analogRead(pinTempSensor);
  float resistance = ((float)R0)*(1023.0/((float)val)-1.0);
  float temp_C = 1.0/(log(resistance/R0)/B+1/298.15)-273.15;
  float temp_F = temp_C*9.0/5.0+32.0;
  Serial.print("temperature = ");
  Serial.println(temp_F);

  if (temp_F < TEMP_THRESHOLD) {
    digitalWrite(pinLED, HIGH);
  } else {
    digitalWrite(pinLED, LOW);
  }
  delay(2000);
}
