#include <../pendant/pendant.h>
#include <../mqttSocket/mqttSocket.h>
#include <../screenDriver/screenDriver.h>
#include <ArduinoJson.h>

#pragma once

class lifewatch{    

    private:
        std::unique_ptr<pendant> pendantobj_;
        std::unique_ptr<screenDriver> screenDriver_;
        mqttSocket client_;
        bool setup;
        unsigned long lastMillis;

        void sendGPS();
        void initWifi(bool debugMode, String SSID, String Pass);
        void sendNudge();
    public:
        lifewatch(std::unique_ptr<memoryManager> mem, Audio* audio, TinyGPSPlus * gps, IPAddress address, bool debugMode, String SSID, String Pass, TFT_eSPI * tft);

        void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
        void setCallback(void (*func)(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total));
        void loop();
};