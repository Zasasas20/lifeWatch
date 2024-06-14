#include "APServer.h"

APServer::APServer(std::unique_ptr<memoryManager> mem) : 
 mem_(std::move(mem)){
    mem_->disableWiFi();

    WiFi.softAPConfig(AP_IP_, AP_GATEWAY_, AP_SUBNET_);

    Serial.println("Mounting SPIFFS");
    
}

void APServer::connect(){
    WiFi.softAP(AP_SSID_, AP_password_);

    Serial.println(WiFi.softAPIP());

    server_.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
        request->send(SPIFFS, "/setup.html", "text/html");
    });

    server_.serveStatic("/", SPIFFS, "/");

    webSocket_.begin();
    webSocket_.onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {this->handler(num, type, payload, length);});

    Serial.println("Websocket Initialized");

    server_.begin();
    Serial.println("Server Initialized");
}

void APServer::loop(){
    webSocket_.loop();
}

void APServer::handler(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
    if (type == WStype_TEXT){
      DeserializationError error = deserializeJson(doc_rx_, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      else {
        const char* ID = doc_rx_["SSID"];
        const char* PASS = doc_rx_["PASSWORD"];
        if(strcmp(ID, "") != 0 && strcmp(PASS, "") != 0){
          mem_->writeNetwork(ID, PASS);
          Serial.println("Stored");
          WiFi.softAPdisconnect(true);
          Serial.println("Disconnected");
          server_.end();
          webSocket_.close();
          Serial.println("Successfully closed");
          delay(200);
          ESP.restart();
        }
      }
  }
}