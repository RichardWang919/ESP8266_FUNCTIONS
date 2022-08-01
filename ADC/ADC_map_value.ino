
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
int outputValue, sensorValue = 0;        // value read from the pot

void setup() {
  Serial.begin(115200);
  
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // map it to the range of the analog out:
  outputValue = map(sensorValue, 0, 1024, 0, 305);
  Serial.printf("mV = %4d\n", outputValue);
  delay(100);
}
