#include "lifewatch.h"

lifewatch::lifewatch(std::unique_ptr<memoryManager> mem, Audio* audio, IPAddress address, bool debugMode, String SSID, String Pass):
client_(mqttSocket(address, "")),
pendantobj_(std::unique_ptr<pendant>(new pendant(std::move(mem), audio))),
screenDriver_(std::unique_ptr<screenDriver>(new screenDriver())){
    initWifi(debugMode, SSID, Pass);
}

void lifewatch::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println(topic);
  String message = client_.getPayload(payload, len);
  if (_setup && strcmp(topic, "Chip/Init") == 0 && strcmp(message.c_str(), "ACK") != 0 && strcmp(message.c_str(), "New Device") != 0){
    Serial.println("Code: ");
    Serial.println(message);
    _setup = false;
    pendantobj_->SetCode(message);
    client_.send("Chip/Init", "ACK");
    delay(1000);
    ESP.restart();
  }
  else if (strcmp(topic, "App/Message") == 0 && !_setup){
    StaticJsonDocument<200> doc_rx_;
    Serial.println(message);
    deserializeJson(doc_rx_, message);
    const char* code = doc_rx_["code"];
    Serial.println(code);
    Serial.println(pendantobj_->getCode());
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
    obj["status"] = _sos? "SOS" : "Normal";
    obj["LocationData"]["Long"] = lng;
    obj["LocationData"]["Lat"] = lat;
    obj["LocationData"]["Mode"] =  "GPS";
    obj["req"] = "Chip";
    serializeJson(doc_tx, jString);
    client_.send("Chip/Message", jString.c_str());
}

void lifewatch::loop(){
    pendantobj_->loop();
    if(_setup && millis() - _lastMillis > 3000) {sendNudge(); _lastMillis = millis();}
    else if (!_setup && _count > 0 && millis() - _lastMillis > 1000 && _mode){
      _lastMillis = millis();
      screenDriver_->displayCode(pendantobj_->getCode());
      screenDriver_->countdownSOS(_count--);
      if(_count == 0){
        screenDriver_->displayCode(pendantobj_->getCode());
        screenDriver_->displaySOS();
        _sos = true;
        pendantobj_->PlayAudio("SOS mode on");
        sendSOS(true);
      }
    }
    else if (!_setup && millis() - _lastMillis > 10000 && _sos){
      sendSOS(true);
      _lastMillis = millis();
      screenDriver_->displayCode(pendantobj_->getCode());
      screenDriver_->displaySOS();
    }
}

void lifewatch::sendNudge(){
    client_.send("Chip/Init", "New Device");
}

void lifewatch::setCallback(void (*func)(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)){
    client_.setHandler(func);
    client_.connect();
    _setup = !pendantobj_->Initialize();
    if (!_setup){screenDriver_->displayCode(pendantobj_->getCode());}
}

void lifewatch::flipMode(){
  if (!_setup){
    _mode = _mode? false: true;
    if (_mode) pendantobj_->PlayAudio("Manual SOS mode in 10 seconds, press button to cancel");
    else{sendSOS(false);
    pendantobj_->PlayAudio("SOS mode cancelled");}
    _sos = false;
    _count = 10;
    if (!_mode) screenDriver_->displayCode(pendantobj_->getCode());
  }
}

void lifewatch::sendSOS(bool mode){
  String jstring = "";
  StaticJsonDocument<200> doc_tx;
  JsonObject obj = doc_tx.to<JsonObject>();
  obj["code"] = pendantobj_->getCode();
  obj["status"] = mode? "SOS" : "Normal";
  serializeJson(doc_tx, jstring);
  client_.send("Chip/SOS", jstring.c_str());
}

void lifewatch::fall(){
  if (!_setup){
    _mode = true;
    pendantobj_->PlayAudio("Fall detected, press the button to cancel SOS mode");
  }
}