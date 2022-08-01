
#define PIN_LED D4
int PWM_value =0;

void setup() {
  pinMode(PIN_LED, OUTPUT); //設置引腳模式
  analogWrite(PIN_LED, 0); 
}

 void loop() { 
  for(int val=0;val<255;val++){ 
    analogWrite(PIN_LED, val);
    delay(15); 
  }
  for(int val=255;val>=0;val--){ 
    analogWrite(PIN_LED, val); 
    delay(15); 
  }
}
