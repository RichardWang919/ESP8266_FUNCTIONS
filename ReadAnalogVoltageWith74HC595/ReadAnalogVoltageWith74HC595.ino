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
0.518 V      0.508 V 
1.655 V      1.657 V 
3.05  V      3.049 V
4.22  V      4.178 V

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
