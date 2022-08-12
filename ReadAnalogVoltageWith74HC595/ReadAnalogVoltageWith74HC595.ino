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
              SHCP | | |  
                   | | |   
                   | | |   
                STCP | |   
                     | |   
                     | |   
                OE GND |
                       |
                       |
                       DS
                   
                  
Meter Real   Get
Voltage      Voltage
---------------------------------------
0.512	0.508	0.004	0.78%
1.731	1.729	0.002	0.12%
3.32	3.307	0.013	0.39%
5.01	5.016	-0.006	-0.12%
7.03	6.97	0.06	0.85%
8.35	8.21	0.14	1.68%
9.06	8.867	0.193	2.13%
10.14	9.95	0.19	1.87%
11.56	11.253	0.307	2.66%

*/

int latchPin = D6;
int clockPin = D7;
int dataPin = D5;

byte led = 0;       // Variable which will save the value of LEDs

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
}

void loop()
{
  led = 0;// at the start all the LEDs will remain in off state
  ShiftRegister(); // turn on the  next LED
  for (int cnt = 0; cnt < 8; cnt++)   // loop that will turn the LED one by one
  {
    led = 1<<cnt;
    //bitSet(led, i); // assigning the respective LED the HIGH values
    ShiftRegister();// turn off the previous LED
    
    int sensorValue = analogRead(A0);
    float voltage = map(sensorValue -34, -20, 1024, 0, 1000) /1000.0 *3.3 *2 ;
    if(voltage <= 0.05)
      voltage =0;

    char temp[32] = "";
    sprintf(temp, "CH-%d: %.3fv" , cnt +1, voltage);
    Serial.println(temp);
    
  }
  Serial.println("----------------");
  delay(500);
}
// function that will update the register after each iteration of for loop
void ShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, MSBFIRST, led);
   digitalWrite(latchPin, HIGH);
   delay(15);
}
