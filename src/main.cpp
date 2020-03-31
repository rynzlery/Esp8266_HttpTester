#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OKLED D0
#define ERRORLED D7
 
const char* ssid = "[SSID]";
const char* password = "[PASSWORD]";

const long blinkInterval = 1000;
const long webRequestInterval = 120000;  //120000
unsigned long previousMillis_LedBlink = 0;   // will store last time LED was updated
unsigned long previousMillis_WebRequest = 0;   // will store last time LED was updated

int okLedState = LOW;
int errorLedState = LOW;

int errorStatusWeb1 = LOW;
int errorStatusWeb2 = LOW;

Adafruit_SSD1306 display(-1);

// DECLARATION
void DisplayText(bool web1, bool web2);
 
void setup () {

  pinMode(OKLED, OUTPUT);
  pinMode(ERRORLED, OUTPUT);
 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.print("Connecting..");
    digitalWrite(ERRORLED, HIGH);
  }

  // initialize with the I2C addr 0x3C
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  // Display Text
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}
 
void loop() {

  unsigned long currentMillis = millis();

  // manage led blinking
  if (currentMillis - previousMillis_LedBlink >= blinkInterval) {
    // save the last time you blinked the LED
    previousMillis_LedBlink = currentMillis;

    if (errorStatusWeb1 == LOW && errorStatusWeb2 == LOW){
      errorLedState = LOW;   // on met à zero la LED ERROR
      if (okLedState == LOW) {
        okLedState = HIGH;
      } else {
        okLedState = LOW;
      }
    }
    else {
      okLedState = HIGH;   // on met à zero la LED OK
      if (errorLedState == LOW){
        errorLedState = HIGH;
      } else {
        errorLedState = LOW;
      } 
    }

    digitalWrite(OKLED, okLedState);
    digitalWrite(ERRORLED, errorLedState);
  }

  // manage web requests
  if (currentMillis - previousMillis_WebRequest >= webRequestInterval) {
    // save the last time you blinked the LED
    previousMillis_WebRequest = currentMillis;
 
    if (WiFi.status() == WL_CONNECTED) {                          //Check WiFi connection status
     
      HTTPClient http;                                            //Declare an object of class HTTPClient
      http.begin("http://houseofbike.fr");                        //Specify request destination
      int httpCode1 = http.GET();                                  //Send the request
      Serial.print(httpCode1);
       
      if (httpCode1 == 200) {                                      //Check the returning code
        errorStatusWeb1 = LOW;                                    // pas d'erreur, tout est OK
      }
      else {
        errorStatusWeb1 = HIGH;                                   // erreur de code HTTP
      }
      http.end();                                                 //Close connection

      HTTPClient http2;  
      http2.begin("http://domainebassenelais.ddns.net");           //Specify request destination
      int httpCode2 = http2.GET();                                  //Send the request
      Serial.print(httpCode2);
       
      if (httpCode2 == 200) {                                      //Check the returning code
        errorStatusWeb2 = LOW;                                    // pas d'erreur, tout est OK
      }
      else {
        errorStatusWeb2 = HIGH;                                   // erreur de code HTTP
      }
      http2.end();                                                 //Close connection
       
      DisplayText(errorStatusWeb1, errorStatusWeb2);
    }
    else {                                                        // pas de WiFi/connexion Internet
      Serial.print("NO INTERNET");
      digitalWrite(ERRORLED, HIGH);
    }
  }
}

void DisplayText(bool web1, bool web2) {
  display.setCursor(0,20);
  display.println("houseofbike.fr");
  display.setCursor(0,30);
  web1 == LOW ? display.println("OK") : display.println("ERROR");
  display.setCursor(0,40);
  display.println("domainebassenelais");
  display.setCursor(0,50);
  web2 == LOW ? display.println("OK") : display.println("ERROR");
  display.display();
}