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
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage[8] = {0.0};
  voltage[0] = sensorValue / (1024.0 / 3.08);
  // print out the value you read:
  Serial.print(sensorValue);
  Serial.print(" , ");
  Serial.println(voltage[0]);

  
  display.clear();
  for(int cnt =0; cnt <= 8; cnt++){
    char temp[64] = "";
    sprintf(temp, "%d:", cnt+1);
    display.drawString((cnt/4) *64, (cnt%4) *16, temp);
    if(voltage[cnt] > 0.0){
      sprintf(temp, "%.2fV", voltage[cnt]);
      display.drawString((cnt/4) *64 +16, (cnt%4) *16, temp);
    }
  }
  // write the buffer to the display
  display.display();
}
