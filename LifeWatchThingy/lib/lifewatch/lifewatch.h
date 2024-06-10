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
        bool _setup;
        unsigned long _lastMillis;
        bool _mode = false;
        bool _sos = false;
        int _count = 10;

        void initWifi(bool debugMode, String SSID, String Pass);
        void sendNudge();
        void sendSOS(bool mode);
    public:
        lifewatch(std::unique_ptr<memoryManager> mem, Audio* audio, IPAddress address, bool debugMode, String SSID, String Pass, TFT_eSPI * tft);

        void flipMode();
        void fall();
        void sendGPS(float lat, float lng);
        void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
        void setCallback(void (*func)(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total));
        void loop();
};