#include <Arduino.h>
#include <TinyGPS++.h>
#include <../lib/webServer/APServer.h>
#include <../lib/memorySocket/memorySocket.h>
#include <../lib/mqttSocket/mqttSocket.h>
#include <Audio.h>

std::unique_ptr<memoryManager> mem = std::unique_ptr<memoryManager>(new memoryManager);

std::unique_ptr<APServer> server;

mqttSocket client = mqttSocket(IPAddress(80,115,229,72));
TFT_eSPI tft = TFT_eSPI();

TinyGPSPlus gps;

Audio audio;

#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

// DEBUG ONLY:
bool debugMode = true;
String SSID = "Zaid";
String Pass = "Holdonbro";

unsigned long lastMillis = 0;

bool setting_up = false;

String code;

void SetUpCode(String code){
  setting_up = false;
  Serial.println("Setting up: ");
  Serial.println(code);
  mem->setCode(code.c_str());
  delay(200);
}

void initWifi(){
  Serial.println("Internet Data is setup");
  if (debugMode){
    WiFi.begin(SSID, Pass);
  }
  else{
    WiFi.begin(mem->getSSID(), mem->getPass());
  }

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

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println(topic);
  String message = client.getPayload(payload, len);
  if (setting_up && strcmp(topic, "Chip/Init") == 0 && strcmp(message.c_str(), "ACK") != 0){
    Serial.println("Code: ");
    Serial.println(message);
    SetUpCode(message);
  }
  else if (strcmp(topic, "App/Message") == 0){
    StaticJsonDocument<200> doc_rx_;
    deserializeJson(doc_rx_, payload);
    if (strcmp(doc_rx_["Code"], code.c_str()) == 0){
      audio.connecttospeech(doc_rx_["Message"], "en");
    }
  }
}

void setup() {

  Serial.begin(9600);

  tft.init();

  Serial2.begin(9600,SERIAL_8N1,25,26);

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  audio.setVolume(10);

  if (mem->isSetup() || debugMode){
    initWifi();
    client.setHandler(onMqttMessage);
    client.connect();
    delay(1000);
    setting_up = !mem->isCodeSetup();
    if (setting_up){
      client.send("Chip/Init", "New Device");
      int ticks = 0;
      tft.fillScreen(TFT_BLACK);
      tft.drawString("Waiting for code", 0, 100);
      while(setting_up){
        ticks++;
        if (ticks == 10){
          ticks = 0;
          client.send("Chip/Init", "New Device");
        }
        delay(1000);
      }
      client.send("Chip/Init", "ACK");
      delay(1000);
      ESP.restart();
    }
    else{
      tft.fillScreen(TFT_BLACK);
      code = mem->getCode();
      tft.drawString(code,0,100);
    }
    client.send("test/test", "connects");
  }
  else{
    Serial.println("Read AP");
    server = std::unique_ptr<APServer>(new APServer(std::move(mem), &tft));
    server->connect();
  }

}

void sendGPS(){
  String jString = "";
  StaticJsonDocument<200> doc_tx;
  JsonObject obj = doc_tx.to<JsonObject>();
  obj["code"] = code;
  obj["battery"] = 9.0;
  obj["status"] = "Normal";
  obj["Location"]["Long"] = gps.location.lng();
  obj["Location"]["Lat"] = gps.location.lat();
  obj["Location"]["Mode"] =  "GPS";
  serializeJson(doc_tx, jString);
  client.send("Chip/Message", jString.c_str());
}

void loop() {
  audio.loop();
  if (server.get() != nullptr){
      server->loop();
  }
  else{
    while(Serial2.available() > 0){
      if (gps.encode(Serial2.read())){
        if (millis() - lastMillis > 5000){
          if(gps.location.isValid()){
            sendGPS();
            lastMillis = millis();
          }
        }
      }
    }
  }
}
