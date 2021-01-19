#include <ESP8266HTTPClient.h> 
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>          
#include <FS.h>                   
#define BLYNK_PRINT Serial
const char* ssid;
const char* pass;
char auth[40] = "q7tq3QZnW5pw9dZHpXxNyy2K86V1eAjp";
bool shouldSaveConfig = false;

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#define PIN            13
#define NUMPIXELS      4
#define TOUCH 14
Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
unsigned long black       = matrix.Color( 0 , 0 , 0 );
unsigned long red         = matrix.Color(255, 0 , 0 );
unsigned long yellow      = matrix.Color(255, 255, 0 );
unsigned long green       = matrix.Color( 0 , 255, 0 );
unsigned long cyan        = matrix.Color( 0 , 255, 255);
unsigned long blue        = matrix.Color( 0 , 0, 255);
unsigned long magenta     = matrix.Color(255, 0 , 255);
unsigned long gray        = matrix.Color(255, 255, 255);
unsigned long white       = matrix.Color(255, 255, 255);
unsigned long orange       = matrix.Color(255, 50, 0);
unsigned long COLORS[7] = {red, yellow, green, cyan, blue, magenta, gray};
int COLOR = 0;

int escihaz=6;
BLYNK_WRITE(7){
  int i=param.asInt(); 
  blurall(black, COLORS[i], 5);
  delay(5000);
  blurall(COLORS[i], black, 5);
    
}

BLYNK_CONNECTED(){
 for(int i=0;i<3;i++){
 blurall(black,green,5);
 delay(500);
 blurall(green,black,5); 
  }
 }
 
void rainbow(int DELAY) {
  blurall(red, green, DELAY);
  blurall(green, blue, DELAY);
  blurall(blue, red, DELAY);
}
void blurall(unsigned long color1, unsigned long color2, int DELAY) {
  unsigned int red1;
  unsigned int green1;
  unsigned int blue1;
  blue1 = (color1 % 256);
  green1 = ((color1 - blue1) % 16711680);
  red1 = (color1 - green1 - blue1) / 65536;
  green1 = green1 / 256;
  unsigned int red2;
  unsigned int green2;
  unsigned long  blue2;
  blue2 = color2 % 256;
  green2 = ((color2 - blue2) % 16711680);
  red2 = (color2 - green2 - blue2) / 65536;
  green2 = green2 / 256;

  while (color1 != color2) {
    if (red1 < red2)
      red1++;
    else if (red1 > red2)
      red1--;

    if (green1 < green2)
      green1++;
    else if (green1 > green2)
      green1--;

    if (blue1 < blue2)
      blue1++;
    else if (blue1 > blue2)
      blue1--;
      
    color1 = red1 * 65536 + green1 * 256 + blue1;
    matrix.fill(color1, 0, 4);
    delay(DELAY);
    matrix.show();

  }
}



unsigned int Sense() {
  unsigned long Counter = 0;
  while (digitalRead(TOUCH) && Counter < 2000) {
    delay(1);
    Counter++;
  }
  return Counter;
}

void TOGLE(){
  HTTPClient http;  
  http.begin("http://139.59.206.133/q7tq3QZnW5pw9dZHpXxNyy2K86V1eAjp/update/V"+String(escihaz)+"?value="+String(COLOR)); 
  int httpCode = http.GET();
  http.end();
  delay(500);
}

void Touch() {
  Serial.println(COLOR);
  
  blurall(black, COLORS[COLOR], 5);
   TOGLE(); 
  delay(5000);
  blurall(COLORS[COLOR], black, 5);
 
}

void Configure() {
  matrix.fill(orange,0,4);
  matrix.show();
  delay(3000);
  matrix.fill(black,0,4);
  matrix.show();
  bool Exit = false;
  while (!Exit) {
    int sonuc = Sense();
    if (sonuc == 2000)
      Exit = true;
    else if (sonuc > 50)
      COLOR++;
    if (COLOR > 6)
      COLOR = 0;
      matrix.fill(COLORS[COLOR],0,4);
      matrix.show();
  }
  EEPROM.write(500, COLOR); 
  EEPROM.commit();
  matrix.fill(orange,0,4);
  matrix.show();
  delay(3000);
  matrix.fill(black,0,4);
  matrix.show();
 

}
void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  COLOR = EEPROM.read(500);
  if (SPIFFS.begin()) {
    if (SPIFFS.exists("/config.json")) {
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(auth, json["authtoken"]);
        } else {
         }
        configFile.close();
      }
    }
  } else {
  }

  matrix.begin();
  matrix.setBrightness(255);
  matrix.fill(black, 0, 4);
  matrix.show();
  pinMode(TOUCH, INPUT);
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  WiFiManagerParameter authtoken("token(degistirmeyin)", "blynk-token", auth, 40);
  wifiManager.addParameter(&authtoken);
  blurall(black,red,5);
  wifiManager.autoConnect("VIBE");
  ssid = WiFi.SSID().c_str();
  pass = WiFi.psk().c_str();
  strcpy(auth, authtoken.getValue());
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["authtoken"] = auth;
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    }
  Serial.println("\tauthtoken : " + String(auth));  
  Blynk.begin(auth, ssid, pass);
  
  
  
}


void loop() {
  Blynk.run();
  int sonuc = Sense();
  if (sonuc == 2000)
    Configure();
  else if (sonuc > 50)
    Touch();
}
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
