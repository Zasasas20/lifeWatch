#include "mqttSocket.h"

mqttSocket::mqttSocket(IPAddress HostIP, const char * password){
    client_.setCredentials("LifeWatch", password);
    client_.setServer(HostIP, 1883);
}

void mqttSocket::setHandler(void (*func)(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)){
    client_.onMessage(func);
}  

void mqttSocket::connect(){
    client_.onConnect([this](bool sessionPresent) {this->onConnect(sessionPresent);});
    client_.connect();
}

String mqttSocket::getPayload(char * data, size_t len){
    String content = "";
    for (size_t i = 0; i < len; i++){
        content.concat(data[i]);
    }
    return content;
}

void mqttSocket::onConnect(bool sessionPresent){
    client_.subscribe("Chip/Init", 0);
    client_.subscribe("App/Message", 0);
}

void mqttSocket::send(const char* topic, const char* data){
    client_.publish(topic, 0, 0, data);
}