#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <TFT_eSPI.h>
#include <../memorySocket/memorySocket.h>


#pragma once

/// @brief Access Point server used to setup wifi creditentials
class APServer{

    private:
        std::unique_ptr<memoryManager> mem_;
        TFT_eSPI * spi_;
        AsyncWebServer server_ = AsyncWebServer(80);
        WebSocketsServer webSocket_ = WebSocketsServer(81);

        StaticJsonDocument<200> doc_rx_;

        IPAddress AP_IP_ = IPAddress(192,167,1,22); // IP address
        IPAddress AP_GATEWAY_ = IPAddress(192,167,1,5); // Gateway
        IPAddress AP_SUBNET_ = IPAddress(255,255,255,0); // Subnet mask

        const char * AP_SSID_ = "LifeWatch"; //SSID for created AP
        const char * AP_password_ = "PASSWORD"; //Password for created AP

        void handler(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

    public:

        APServer(std::unique_ptr<memoryManager> mem, TFT_eSPI * spi);

        void connect();
        void loop();
};