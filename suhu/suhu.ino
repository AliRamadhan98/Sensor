#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 13     
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

const char *SSID = "Friday";
const char *PSWD = "namira2003";
char jsonOutput [128];

String addSuhu = "https://suhu-web.herokuapp.com/suhu/add";
const char *bearer_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZCI6IjM4Yjg2ZTFmLTNkM2EtNDQ4Zi1iMmVkLTRmZjkxNjNmOTdjMSIsImVtYWlsIjoidGhlb0BnbWFpbC5jb20iLCJwaG9uZU51bWJlciI6IjA4OTk4MTY1NTQ1IiwiaWF0IjoxNjY2MTkyMTM3LCJleHAiOjE2OTc3MjgxMzd9.JzqbKImGFXB1k3fAKy2kvO4Nf6NDVBiw5Z0s3sPZHrw";

HTTPClient client;
//jeda waktu untuk baca sensor
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PSWD);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println(F("DHTxx test!"));

  dht.begin();
}

void loop() {
dhtPost();
}

void dhtPost(){
   if ((millis() - lastTime) > timerDelay){
     if(WiFi.status()== WL_CONNECTED){
      String serverPath = addSuhu;
      client.begin(serverPath.c_str());
      client.setAuthorization(bearer_token);      
      // client.addHeader("Content-Type", "application/json");
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }
      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("%  Temperature: "));
      Serial.print(t);
      Serial.print(F(" Celcius "));
      Serial.println("");

      const size_t CAPACITY = JSON_OBJECT_SIZE(4);
      StaticJsonDocument<CAPACITY> doc;
      JsonObject object = doc.to<JsonObject>();
      object ["suhu"] = t;
      object ["kelembaban"] = h;      
      object ["lat"] ="0" ;
      object ["lon"] = "0";      

      serializeJson(doc, jsonOutput);
      int httpCode = client.POST(String(jsonOutput));

      if (httpCode > 0){
        String payload = client.getString();
        Serial.println("\nStatuscode: " + String(httpCode));
        Serial.println(payload);
        client.end();
      }else {
        Serial.println("Error on HTTP request");
      }

    }else {
      Serial.println("WiFi Disconnected");
      WiFi.disconnect();
      WiFi.reconnect();
      }
      
    lastTime = millis();     
  }
}