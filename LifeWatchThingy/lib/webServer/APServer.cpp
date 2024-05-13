#include "APServer.h"

APServer::APServer(std::unique_ptr<memoryManager> mem, TFT_eSPI * spi) : 
 mem_(std::move(mem)){
    mem_->disableWiFi();

    WiFi.softAPConfig(AP_IP_, AP_GATEWAY_, AP_SUBNET_);

    Serial.println("Mounting SPIFFS");
    
    spi_ = spi;

    if(!SPIFFS.begin()){
      spi_->fillScreen(TFT_BLACK);
      spi_->drawString("Failed to initialize SPIFFS", 0, 100);
      Serial.println("Failed to initialize SPIFFS");
    }
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
    spi_->fillScreen(TFT_BLACK);
    spi_->drawString("Websocket Initialized", 0, 100);
    Serial.println("Websocket Initialized");

    server_.begin();
    Serial.println("Server Initialized");
    spi_->fillScreen(TFT_BLACK);
    spi_->drawString("Server Initialized", 0, 100);

    spi_->fillScreen(TFT_BLACK);
    spi_->drawString("IP: ", 0, 100);
    spi_->drawString(WiFi.softAPIP().toString(), 0, 110);
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
          spi_->fillScreen(TFT_BLACK);
          spi_->drawString("Data Recieved", 0, 100);
          delay(200);
          ESP.restart();
        }
      }
  }
}