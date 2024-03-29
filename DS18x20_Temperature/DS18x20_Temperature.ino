#define DS18B20_Pin D2
#define SG90_Pin D5
#define LED_Pin D4
#define Flash_Button_Pin D3

#define Pos_Zero 190
#define Pos_Max 30

float g_CelsiusLimit = 32.0;
float g_Celsius = 0.0;
int g_FlashButtonStatus = 1;
int g_MotorStatus = 0;
int g_DoorStatus = 0;

// ***********************
//  For DS18B20
// ***********************
#include <OneWire.h>
OneWire  ds(DS18B20_Pin);  // on pin 10 (a 4.7K resistor is necessary)

// ***********************
//  For SG90 Servo
// ***********************
#include <Servo.h>
Servo Servo_SG90;  // create servo object to control a servo

// ***********************
//  For ESP Soft AP Mode
// ***********************
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef APSSID
#define APSSID "ESPap"
#define APPSK  "12345678"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);


// ***********************
//  For SPIFFS
// ***********************
#include <FS.h> 

using namespace std;

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  Serial.println("handleFileRead: " + path); // 在序列埠顯示路徑

  if (path.endsWith("/")) {
    path += "index.htm";
  }

  String contentType = getContentType(path);
  
  if (SPIFFS.exists(path)){
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();

    return true;
  }
  return false;
}

void init_AP_mode() {
  delay(1000);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  //NEW
  SPIFFS.begin(); 

  Serial.println("HTTP server started");

  delay(1000);
  if (!MDNS.begin("TempDetector")) {
    Serial.println("Error setting up MDNS responder!");
  }
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);


  server.begin(); 
  server.on("/", handleSpecificArg);
  server.on("/led_set", led_control);
  server.on("/temp_set", temp_control);
  server.on("/door_set", door_control);
  server.on("/read_temp_limit", sensor_limit_data);
  server.on("/read_temp", sensor_data);
  server.on("/read_motor", motor_data);
  server.on("/read_door", door_data);
  server.on("/read_button", button_data);
  
  server.onNotFound([](){
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });
}

void temp_control() 
{
  String state = "OFF";
  String act_state = server.arg("state");
  g_CelsiusLimit = act_state.toFloat();
  if(act_state == "1"){
    //digitalWrite(LED_Pin, HIGH); //LED ON
    state = "ON";
  }else{
    //digitalWrite(LED_Pin, LOW); //LED OFF
    state = "OFF";
  }
  server.send(200, "text/plane", state);
}

void led_control() 
{
  String state = "OFF";
  String act_state = server.arg("state");
  if(act_state == "1"){
    digitalWrite(LED_Pin, HIGH); //LED ON
    state = "ON";
    g_MotorStatus = 1;
  }else{
    digitalWrite(LED_Pin, LOW); //LED OFF
    state = "OFF";
    g_MotorStatus = 0;
  }
  server.send(200, "text/plane", state);
}

void door_control() 
{
  String act_state = server.arg("state");
  if(act_state == "1"){
    Servo_SG90.write(Pos_Max); // 一開始先置中90度
    g_DoorStatus = 1;
  }else{
    Servo_SG90.write(Pos_Zero); // 一開始先置中0度
    g_DoorStatus = 0;
  }
  server.send(200, "text/plane", act_state);
}

void sensor_limit_data()
{
  char buf[10];
  sprintf(buf, "%.1f", g_CelsiusLimit);
  String sensor_value = buf;
  server.send(200, "text/plane", sensor_value);
}

void sensor_data()
{
  char buf[10];
  sprintf(buf, "%.1f", g_Celsius);
  String sensor_value = buf;
  server.send(200, "text/plane", sensor_value);
}

void button_data()
{
  String sensor_value = String(g_FlashButtonStatus);
  server.send(200, "text/plane", sensor_value);
}

void motor_data()
{
  String sensor_value = String(g_MotorStatus);
  server.send(200, "text/plane", sensor_value);
}

void door_data()
{
  String sensor_value = String(g_DoorStatus);
  server.send(200, "text/plane", sensor_value);
}

void handleSpecificArg() { 
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    // here begin chunked transfer
    server.send(200, "text/html", "");



    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
      Serial.println("Failed to open file for reading");
      return;
    }
  
    vector<String> v;
    while (file.available()) {
      v.push_back(file.readStringUntil('\n'));
    }
    file.close();
  
    for (String s : v) {
      server.sendContent(s);
    }



    String message = "";
    char msg[200] = {0};  //<head><meta http-equiv=\"Refresh\" content=\"15\">
    //sprintf(msg , "<div class=\"alert alert-dark\" role=\"alert\">目前溫度： <a href=\"#\" class=\"alert-link\">%.1f</a></div>\0", g_Celsius);
    //sprintf(msg , "<div class=\"alert alert-dark\" role=\"alert\">按鈕狀態： <a href=\"#\" class=\"alert-link\">%s</a></div>\0", (g_FlashButtonStatus==0)?"按下中":"放開中" );
    //server.sendContent(msg);
    
    server.sendContent("</div></body></html>");
    server.client().stop();

}

void load_SPIFFS_file(String filename, String file_type)
{
  File file = SPIFFS.open(filename, "r");
  size_t sent = server.streamFile(file, file_type);
}

void AP_Web_process_loop() {
  server.handleClient();
  MDNS.update();
}

void setup(void) {
  Serial.begin(9600);
  pinMode(DS18B20_Pin, INPUT_PULLUP);
  pinMode(SG90_Pin, OUTPUT);
  pinMode(LED_Pin, OUTPUT);
  pinMode(Flash_Button_Pin, INPUT_PULLUP);

  //Servo_SG90.attach(SG90_Pin); // 修正脈衝寬度範圍
  Servo_SG90.attach(SG90_Pin, 500, 2400); // 修正脈衝寬度範圍
  delay(2000);

  //ServoInitTest();

  //  Go into AP mode if button is pressing when system boot up.
  //if ( digitalRead(Flash_Button_Pin) == 0 ) {
    init_AP_mode();
  //}
}

void ServoInitTest() {

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  Servo_SG90.write(Pos_Zero); // 一開始先置中0度
  delay(500);

  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  Servo_SG90.write(Pos_Max); // 一開始先置中90度
  delay(500);

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  Servo_SG90.write(Pos_Zero); // 一開始先置中0度
}

void loop(void) {
  g_FlashButtonStatus = GetFlashButtonStatus();
  g_Celsius = 30.5; //Get_Temp_Value();
  AP_Web_process_loop();

  if(g_Celsius >= g_CelsiusLimit) {
    Servo_SG90.write(Pos_Max);
    g_DoorStatus = 1;
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(50);
  }

  if(g_FlashButtonStatus ==0){
    Servo_SG90.write(Pos_Max);
    g_DoorStatus = 1;
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(5000);
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    Servo_SG90.write(Pos_Zero);
    delay(500);
    g_DoorStatus = 0;
  }
  
  /*
  while (g_Celsius >= 45.0) {
    while (true) {
      delay(1000);
    }
  }
  */
}

int GetFlashButtonStatus(){
  return digitalRead(Flash_Button_Pin);
}

float Get_Temp_Value() {

  int i;
  int present = 0;
  int type_s;
  unsigned char data[9];
  unsigned char addr[8];
  float celsius, fahrenheit;

  if ( !ds.search(addr)) {
    //Serial.println("Warning: No more addresses.");
    //Serial.println();
    ds.reset_search();
    delay(250);
    return -1.0;
  }

  /*
    Serial.print("ROM =");
    for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
    }
  */

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("Error: CRC is not valid!");
    return -1.0;
  }
  Serial.println();

  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      //Serial.println("System: Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("System: Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("System: Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Error: Device is not a DS18x20 family device.");
      return -1.0;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("System: Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    int cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("Temperature: ");
  Serial.println(celsius);
  //Serial.print(" Celsius, ");
  //Serial.print(fahrenheit);
  //Serial.println(" Fahrenheit");
  delay(1000);
  return celsius;
}
