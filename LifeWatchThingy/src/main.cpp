#include <Arduino.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>  
#include <../lib/webServer/APServer.h>
#include <../lib/memorySocket/memorySocket.h>

std::unique_ptr<memoryManager> mem = std::unique_ptr<memoryManager>(new memoryManager);

std::unique_ptr<APServer> server;

TFT_eSPI tft = TFT_eSPI();

void setup() {

  Serial.begin(9600);

  tft.init();

  if (mem->isSetup()){
    Serial.println("Internet Data is setup");
    WiFi.begin(mem->getSSID(), mem->getPass());

    int count=0;

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.println("Connecting");
        count++;

        if (count >= 10){
          WiFi.disconnect();
          mem->disableWiFi();
          Serial.println("Connection Failed, restarting in AP Mode");
          delay(1000);
          ESP.restart();
        }
    }
        Serial.print("Connected on: ");
        Serial.println(WiFi.localIP());

        tft.fillScreen(TFT_BLACK);
        tft.drawString("Connected to Wifi", 0, 100);
  }
  else{
    Serial.println("Read AP");
    server = std::unique_ptr<APServer>(new APServer(std::move(mem), &tft));
  }

  server->connect();
}

void loop() {
  server->loop();
}
