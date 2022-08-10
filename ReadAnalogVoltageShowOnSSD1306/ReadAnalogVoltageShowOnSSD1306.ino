/*
  ReadAnalogVoltage

GND ---- 10K ----|---- 10K ---- Detecect Pin
                 |
                 |
            2N2222
A0 ---------|  |
               |
              1M ---------- Enable Pin
            
                   
                  
Meter Real   Get
Voltage      Voltage
---------------------------------------
0.512 V      0.508 V      -0.8%
1.001 V      1.009 V       0.8%
1.731 V      1.729 V      -0.1%
3.32  V      3.307 V      -0.4%
4.12  V      4.085 V      -0.8%

*/

#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

SSD1306Wire display(0x3c, D6, D5);  // ADDRESS, SDA, SCL  -  If not, they can be specified manually.

int CH_01 = D2;
int CH_02 = D7;

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

  pinMode(CH_01, OUTPUT);
  pinMode(CH_02, OUTPUT);
  digitalWrite(CH_01, LOW);
}

// the loop routine runs over and over again forever:
void loop() {
  int sensorValue;
  float voltage[8] = {0.0};
  
  digitalWrite(CH_01, HIGH);
  digitalWrite(CH_02, LOW);
  delay(5);
  
  // read the input on analog pin 0:
  sensorValue = analogRead(A0);
  voltage[0] = map(sensorValue -33, -20, 1024, 0, 1000) /1000.0 *3.3 *2 ;



  digitalWrite(CH_01, LOW);
  digitalWrite(CH_02, HIGH);
  delay(5);
  
  // read the input on analog pin 0:
  sensorValue = analogRead(A0);
  voltage[1] = map(sensorValue -33, -20, 1024, 0, 1000) /1000.0 *3.3 *2 ;


  digitalWrite(CH_01, LOW);
  digitalWrite(CH_02, LOW);
  delay(100);
  
  display.clear();
  for(int cnt =0; cnt <= 8; cnt++){
    char temp[64] = "";
    sprintf(temp, "%d:", cnt+1);
    display.drawString((cnt/4) *64, (cnt%4) *16, temp);
    if(voltage[cnt] > 0.05){
      sprintf(temp, "%.3fv", voltage[cnt]);
      display.drawString((cnt/4) *64 +14, (cnt%4) *16, temp);
      
      Serial.print("CH");
      Serial.print(cnt);
      Serial.print(" , ");
      Serial.println(voltage[cnt]);
    }
  }
  // write the buffer to the display
  display.display();
}
