#include "lifewatch.h"

lifewatch::lifewatch(std::unique_ptr<memoryManager> mem, Audio* audio, IPAddress address, bool debugMode, String SSID, String Pass, TFT_eSPI * tft):
client_(mqttSocket(address, "")),
pendantobj_(std::unique_ptr<pendant>(new pendant(std::move(mem), audio))),
screenDriver_(std::unique_ptr<screenDriver>(new screenDriver(tft))){
    initWifi(debugMode, SSID, Pass);
}

void lifewatch::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println(topic);
  String message = client_.getPayload(payload, len);
  if (setup && strcmp(topic, "Chip/Init") == 0 && strcmp(message.c_str(), "ACK") != 0 && strcmp(message.c_str(), "New Device") != 0){
    Serial.println("Code: ");
    Serial.println(message);
    setup = false;
    pendantobj_->SetCode(message);
    client_.send("Chip/Init", "ACK");
    delay(1000);
    ESP.restart();
  }
  else if (strcmp(topic, "App/Message") == 0 && !setup){
    StaticJsonDocument<200> doc_rx_;
    Serial.println(message);
    deserializeJson(doc_rx_, message);
    const char* code = doc_rx_["code"];
    Serial.println(code);
    if (strcmp(code, pendantobj_->getCode().c_str()) == 0){
      const char* voicemessage = doc_rx_["message"];
      Serial.println(voicemessage);
      pendantobj_->PlayAudio(voicemessage);
    }
  }
}

void lifewatch::initWifi(bool debugMode, String SSID, String Pass){
  Serial.println("Internet Data is setup");
  if (debugMode){
    WiFi.begin(SSID, Pass);
  }
  else{
    WiFi.begin(pendantobj_->mem->getSSID(), pendantobj_->mem->getPass());
  }

  int count=0;

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Connecting");
    count++;

    if (count >= 10){
      WiFi.disconnect();
      pendantobj_->mem->disableWiFi();
      Serial.println("Connection Failed, restarting in AP Mode");
      delay(1000);
      ESP.restart();
      }
  }
    Serial.print("Connected on: ");
    Serial.println(WiFi.localIP());
}

void lifewatch::sendGPS(float lat, float lng){
    String jString = "";
    StaticJsonDocument<200> doc_tx;
    JsonObject obj = doc_tx.to<JsonObject>();
    obj["code"] = pendantobj_->getCode();
    obj["battery"] = 9.0;
    obj["status"] = "Normal";
    obj["LocationData"]["Long"] = lng;
    obj["LocationData"]["Lat"] = lat;
    obj["LocationData"]["Mode"] =  "GPS";
    obj["req"] = "Chip";
    serializeJson(doc_tx, jString);
    client_.send("Chip/Message", jString.c_str());
}

void lifewatch::loop(){
    pendantobj_->loop();
}

void lifewatch::sendNudge(){
    client_.send("Chip/Init", "New Device");
}

void lifewatch::setCallback(void (*func)(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)){
    client_.setHandler(func);
    client_.connect();
    setup = !pendantobj_->Initialize();
    if (!setup){screenDriver_->displayCode(pendantobj_->getCode());}
}