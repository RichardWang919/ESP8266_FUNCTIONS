/*
  ReadAnalogVoltageWith 74HC595

GND ---- 10K ----|---- 10K ---- Detecect Pin CH 8
                 |
                 |
            2N2222
A0 ---------|  |
               |
              1M
               |
               |Q7 CH 8 Enable Pin --- 1M --- Other 2N2222 and Detecect Pin
               | Q6 CH 7 Enable Pin --- 1M --- Other 2N2222 and Detecect Pin
               | | Q5 CH 6 Enable Pin --- 1M --- Other 2N2222 and Detecect Pin
               | | | Q4 CH 5 Enable Pin --- 1M --- Other 2N2222 and Detecect Pin
               | | | | Q3 CH 4 Enable Pin --- 1M --- Other 2N2222 and Detecect Pin
               | | | | | Q2 CH 3 Enable Pin --- 1M --- Other 2N2222 and Detecect Pin
               | | | | | | Q1 CH 2 Enable Pin --- 1M --- Other 2N2222 and Detecect Pin
     GND-----| | | | | | | |
             ----------------
                 74HC595    C
             ----------------
             | | | | | | | |
  Next 74HC595 | | | | | | VCC
               | | | | | |
               | | | | | |
          MR VCC | | | | Q0 CH 1 Enable Pin --- 1M --- Other 2N2222 and Detecect Pin
                 | | | |
                 | | | |
     SHCP to MCU | | |
                   | | |
                   | | |
       STCP to MCU | |
                     | |
                     | |
                OE GND |
                       |
                       |
              DS to MCU
                   
                  
Meter Real   Get
Voltage      Voltage
---------------------------------------
0.512  0.508 0.004 0.78%
1.731 1.729 0.002 0.12%
3.32  3.307 0.013 0.39%
5.01  5.016 -0.006  -0.12%
7.03  6.97  0.06  0.85%
8.35  8.21  0.14  1.68%
9.06  8.867 0.193 2.13%
10.14 9.95  0.19  1.87%
11.56 11.253  0.307 2.66%

*/

float voltage[8] = {0.0};
  
//  For OLED Display
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"
SSD1306Wire display(0x3c, D3, D2);  // ADDRESS, SDA, SCL  -  If not, they can be specified manually.

//  For read voltage with 74HC595
int latchPin = D6;
int clockPin = D7;
int dataPin = D5;
byte value_74HC595 = 0;

void setup()
{
// assigning the working modes to the pins of shift register
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);

  Serial.begin(9600);
  Serial.println();
  Serial.flush();
  Serial.println();
  
  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
}

void loop()
{
  value_74HC595 = 0;// at the start all the detect pin will remain in off state
  ShiftRegister(); // turn on the  next detect pin
  for (int cnt = 0; cnt < 8; cnt++)   // loop that will turn the detect pin one by one
  {
    value_74HC595 = 1<<cnt;
    //bitSet(led, i); // assigning the respective LED the HIGH values
    ShiftRegister();// turn off the previous detect pin
    
    //voltage[cnt] = map(sensorValue -34, -20, 1024, 0, 1000) /1000.0 *3.3 *5 ;
    float last_voltage = voltage[cnt];
    int sensorValue = analogRead(A0);
    voltage[cnt] = last_voltage + map(sensorValue -17, 0, 1024, 0, 1000) /1000.0 *3.3 *5 ;
    
    if(voltage[cnt] <= 0.05)
      voltage[cnt] =0;

    char temp[32] = "";
    sprintf(temp, "CH-%d: %.2fv" , cnt +1, voltage[cnt]);
    Serial.println(temp);
    
  }
  Serial.println("----------------");


  
  display.clear();
  for(int cnt =0; cnt <= 8; cnt++){
    char temp[64] = "";
    sprintf(temp, "%d:", cnt+1);
    display.drawString((cnt/4) *64, (cnt%4) *16, temp);
    //if(voltage[cnt] > 0.05)
    {
      if(voltage[cnt] > 10.0)
        sprintf(temp, "%.2fv", voltage[cnt]);
      else
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

  
  //delay(500);
}
// function that will update the register after each iteration of for loop
void ShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, MSBFIRST, value_74HC595);
   digitalWrite(latchPin, HIGH);
   delay(15);
}
