#include <AsyncMqttClient.h>

#pragma once

class mqttSocket{

private:
    AsyncMqttClient client_;
    void onConnect(bool sessionPresent);

public:
    mqttSocket(IPAddress HostIP, const char * password = nullptr);

    void setHandler(void (*func)(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total));  
    void connect();
    String getPayload(char * data, size_t len);
    void send(const char* topic, const char* data);
};