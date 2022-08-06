#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "testuict@unifi";
const char* password = "";
// const char* ssid = "Redmi Note 10S";
// const char* password = "1234567890";

// const String apikey = "apikey=5211740526";
// const String device = "5211740526_device_1";
const String apikey = "apikey=8116843801";
const String device = "8116843801_device_1";

String serverName = "http://api.dmsolution2u.com/api/devices?apikey=8116843801&device=8116843801_device_1";
String serverPost = "http://api.dmsolution2u.com/api/streams/click?" + apikey;

// #define RELAY_PIN_1 16
// #define RELAY_PIN_2 17
// #define RELAY_PIN_3 18
// #define RELAY_PIN_4 19
// #define RELAY_PIN_5 26
#define RELAY_PIN_1 16
#define RELAY_PIN_2 15
#define RELAY_PIN_3 14
#define RELAY_PIN_4 13
#define RELAY_PIN_5 12

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(9600); 

  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);
  pinMode(RELAY_PIN_4, OUTPUT);
  pinMode(RELAY_PIN_5, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();


        char json[500];
        payload.toCharArray(json, 500);

        StaticJsonDocument<600> doc;
        deserializeJson(doc, json);

        // digitalWrite(RELAY_PIN_5, HIGH);
        // Serial.println("Relay On");
        // delay(5000);
        // digitalWrite(RELAY_PIN_5, LOW);
        // Serial.println("Relay Off");
        // delay(5000);

        for(int i = 0; i < 2; i++) {
          int status = doc["result"][i]["status"];
          // String timer = doc["result"][i]["timer"];
          int time_in = doc["result"][i]["timer"]["time_in"];
          int time_out = doc["result"][i]["timer"]["time_out"];

          http.begin(serverPost);

          http.addHeader("Content-Type", "application/json");

          if(i == 0) {
            if(status == 1) {
              Serial.println("Tank 1 - Valve On");
              digitalWrite(RELAY_PIN_1, HIGH);
              delay(time_out * 1000 * 60);
              digitalWrite(RELAY_PIN_1, LOW);

              digitalWrite(RELAY_PIN_5, HIGH);
              digitalWrite(RELAY_PIN_3, HIGH);
              delay(time_in * 1000 * 60);
              digitalWrite(RELAY_PIN_5, LOW);
              digitalWrite(RELAY_PIN_3, LOW);
              int httpResponseCode = http.POST("{\"button_id\":\"1\"}");
            }
          } else if(i == 1) {
            if(status == 1) {
              Serial.println("Tank 2 - Valve On");
              digitalWrite(RELAY_PIN_2, HIGH);
              delay(time_out * 1000 * 60);
              digitalWrite(RELAY_PIN_2, LOW);

              digitalWrite(RELAY_PIN_5, HIGH);
              digitalWrite(RELAY_PIN_4, HIGH);
              delay(time_in * 1000 * 60);
              digitalWrite(RELAY_PIN_5, LOW);
              digitalWrite(RELAY_PIN_4, LOW);
              int httpResponseCode = http.POST("{\"button_id\":\"2\"}");
            }
          }
        }


      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}