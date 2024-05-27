#include <Arduino.h>
#include <../lib/webServer/APServer.h>
#include <../lib/pendant/pendant.h>
#include <../lib/lifewatch/lifewatch.h>

std::unique_ptr<memoryManager> mem = std::unique_ptr<memoryManager>(new memoryManager);
std::unique_ptr<APServer> server;
std::unique_ptr<lifewatch> LifeWatch;
TinyGPSPlus gps;
Audio audio;

TFT_eSPI tft;

#define I2S_DOUT 12
#define I2S_BCLK 13
#define I2S_LRC 15

// DEBUG ONLY:
bool debugMode = true;
String SSID = "Zaid";
String Pass = "Holdonbro";

void mqttWrapper(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total){
  LifeWatch->onMqttMessage(topic, payload, properties, len, index, total);
}

void setup() {

  Serial.begin(9600);

  Serial2.begin(9600,SERIAL_8N1,25,26);

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  audio.setVolume(10);

  tft.init();

  tft.fillScreen(TFT_BLACK);
  tft.drawString("HIII", 0, 100);

  if (mem->isSetup() || debugMode){
    LifeWatch = std::unique_ptr<lifewatch>(new lifewatch(std::move(mem), &audio, &gps, IPAddress(80,115,229,72), debugMode, SSID, Pass, &tft));
    LifeWatch->setCallback(mqttWrapper);
  }
  else{
    Serial.println("Read AP");
    server = std::unique_ptr<APServer>(new APServer(std::move(mem), &tft));
    server->connect();
  }

}

void loop() {
  if (server.get() != nullptr){
    server->loop();
  }
  else{
    LifeWatch->loop();

  }
}
