/*
  ReadAnalogVoltage

  Reads an analog input on pin 0, converts it to voltage, and prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/ReadAnalogVoltage
*/

#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

SSD1306Wire display(0x3c, D3, D5);  // ADDRESS, SDA, SCL  -  If not, they can be specified manually.

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println();
  Serial.println();


  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);

  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  digitalWrite(D7, LOW);
}

// the loop routine runs over and over again forever:
void loop() {
  int sensorValue;
  float voltage[8] = {0.0};
  
  digitalWrite(D7, LOW);
  digitalWrite(D6, HIGH);
  delay(5);
  
  // read the input on analog pin 0:
  sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  
  //voltage[0] = sensorValue / (1024.0 / 3.19) *2;
  voltage[0] = map(sensorValue -41, -20, 1024, 0, 1000) /1000.0 *3.3 *2 ;
  // print out the value you read:
  Serial.print(sensorValue);
  Serial.print(" , ");
  Serial.println(voltage[0]);




/**/
  digitalWrite(D6, LOW);
  digitalWrite(D7, HIGH);
  delay(5);
  
  // read the input on analog pin 0:
  sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  //voltage[1] = sensorValue / (1024.0 / 3.14) *2;
  voltage[1] = map(sensorValue -41, -20, 1024, 0, 1000) /1000.0 *3.3 *2 ;
  // print out the value you read:
  Serial.print(sensorValue);
  Serial.print(" , ");
  Serial.println(voltage[1]);


  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  delay(100);
  
  display.clear();
  for(int cnt =0; cnt <= 8; cnt++){
    char temp[64] = "";
    sprintf(temp, "%d:", cnt+1);
    display.drawString((cnt/4) *64, (cnt%4) *16, temp);
    if(voltage[cnt] > 0.01){
      sprintf(temp, "%.3fv", voltage[cnt]);
      display.drawString((cnt/4) *64 +14, (cnt%4) *16, temp);
    }
  }
  // write the buffer to the display
  display.display();
}
