

//kütüphaneler

#include <ESP8266HTTPClient.h> 
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>          
#include <FS.h>                   
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#define BLYNK_PRINT Serial
//tanımlamalar
#define ledpin 13 
#define NUMPIXELS 4
Adafruit_NeoPixel pixels(NUMPIXELS, ledpin, NEO_GRB + NEO_KHZ800);
#define sensor 14
const char* ssid;
const char* pass;
char auth[40] = "q7tq3QZnW5pw9dZHpXxNyy2K86V1eAjp";
int dokunma = 0;
unsigned long algilama_araligi_baslangic = 0;
unsigned long algilama_araligi = 0;
unsigned short renk=0;
bool shouldSaveConfig = false;
unsigned short red=250,green=0,blue=0;
//cihazların belirlenmesi
int escihaz=7;
BLYNK_WRITE(6){
  int i=param.asInt();
  renkal(i);
  yak();
  delay(3000);
  sondur();
  renkal(renk);  
}
//tetiklemeyi yapan fonksiyon
void tetik(){
  HTTPClient http;  
  http.begin("http://139.59.206.133/q7tq3QZnW5pw9dZHpXxNyy2K86V1eAjp/update/V"+String(escihaz)+"?value="+String(renk)); 
  int httpCode = http.GET();
  http.end();
  delay(500);
}

BLYNK_CONNECTED(){
  for(int i=0;i<3;i++){
 led(0,250,0,500);
 delay(500);
  }
 }

void setup()
{
  Serial.begin(9600);
  EEPROM.begin(512);
  renk = EEPROM.read(500);
  renkal(renk);
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
  pinMode(sensor, INPUT);
  pinMode(ledpin, OUTPUT);
  pixels.begin();
  pixels.setBrightness(250); 
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  WiFiManagerParameter authtoken("token(degistirmeyin)", "blynk-token", auth, 40);
  wifiManager.addParameter(&authtoken);
  renk2(250,0,0);
  
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
void loop(){
  Blynk.run();
 if (digitalRead(14) == 1)
  {
    if (dokunma == 0)
    {
      algilama_araligi_baslangic = algilama_araligi = millis();
      dokunma++;
    }
    else if (dokunma > 0 && millis()-algilama_araligi >= 150)
    {
      algilama_araligi_baslangic = algilama_araligi = millis();
      dokunma++;
    }
  }
  if (millis()-algilama_araligi_baslangic >= 500||dokunma==10)
  {
    if (dokunma == 2)
    {
    yak();
    tetik();
    delay(3000);
    sondur();
    }
    if(dokunma ==10){
    configure();
    }
    dokunma = 0;
  }
}

void led(int red,int green,int blue,int sec){
  for(int i=0; i<NUMPIXELS; i++) {
  pixels.setPixelColor(i, pixels.Color(red, green, blue));  
  }
  pixels.show(); 
  delay(sec);
  pixels.clear();
  pixels.show();
}
void yak(){
  for(int i=0; i<NUMPIXELS; i++) {
  pixels.setPixelColor(i, pixels.Color(red, green, blue));  
  }
  pixels.show(); 
  }
void sondur(){
  for(int i=0; i<NUMPIXELS; i++) {
  pixels.setPixelColor(i, pixels.Color(0, 0, 0));  
  }
  pixels.show(); 
  }

void renk2(int red,int green,int blue){
  for(int i=0; i<NUMPIXELS; i++) {
  pixels.setPixelColor(i, pixels.Color(red, green, blue));  
  }
  pixels.show(); 
  }
  
void configure(){
  led(250,50,0,4000);
  int durum=0;
  dokunma=0;
  while(durum !=2){
    Blynk.run();
    if (digitalRead(14) == 1)
  {
    if (dokunma == 0)
    {
      algilama_araligi_baslangic = algilama_araligi = millis();
      dokunma++;
    }
    else if (dokunma > 0 && millis()-algilama_araligi >= 150)
    {
      algilama_araligi_baslangic = algilama_araligi = millis();
      dokunma++;
    }
  }
 
  if (millis()-algilama_araligi_baslangic >= 500||dokunma==10)
  {
    if (dokunma == 2)
    {
    renk++;
    }
    if(dokunma ==10){
    durum=2;
    }
    dokunma = 0;
  }
  if(renk>6){
      renk=0;
      }
      if(renk==0){
        red=250;
        green=0;
        blue=0;
        }
      if(renk==1){
        red=250;
        green=250;
        blue=0;
        }
       if(renk==2){
        red=0;
        green=250;
        blue=0;
        }
       if(renk==3){
        red=0;
        green=250;
        blue=250;
        }
        if(renk==4){
        red=0;
        green=0;
        blue=250;
        }
        if(renk==5){
        red=250;
        green=0;
        blue=250;
        }
        if(renk==6){
        red=250;
        green=250;
        blue=250;
        }
    yak();
      
    yield();
      }
    
    EEPROM.write(500, renk); 
    EEPROM.commit();
    led(250,50,0,4000);
    yield();
  }

  void renkal(int renk){
    if(renk>6){
      renk=0;
      }
      if(renk==0){
        red=250;
        green=0;
        blue=0;
        }
      if(renk==1){
        red=250;
        green=250;
        blue=0;
        }
       if(renk==2){
        red=0;
        green=250;
        blue=0;
        }
       if(renk==3){
        red=0;
        green=250;
        blue=250;
        }
        if(renk==4){
        red=0;
        green=0;
        blue=250;
        }
        if(renk==5){
        red=250;
        green=0;
        blue=250;
        }
        if(renk==6){
        red=250;
        green=250;
        blue=250;
        }
    }
 void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
