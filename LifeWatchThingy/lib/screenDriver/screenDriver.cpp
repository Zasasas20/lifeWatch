#include "screenDriver.h"

screenDriver::screenDriver(Adafruit_SSD1306 * sc){screen_ = sc;}

void screenDriver::setup(){
    screen_->clearDisplay();
    screen_->setTextSize(1);
    screen_->setTextColor(WHITE);
    screen_->setCursor(0,0);
}

void screenDriver::displayCode(String code){
    setup();
    screen_->println("Pairing Code: ");
    screen_->println(code);
    screen_->display();
}

void screenDriver::countdownSOS(int num){
    setup();
    screen_->println("SOS mode in: ");
    screen_->println(num);
    screen_->display();
}

void screenDriver::displaySOS(){
    setup();
    screen_->println("SOS MODE ON");
    screen_->display();
}

void screenDriver::connected(String IP){
    setup();
    screen_->println("Connected to: ");
    screen_->println(IP);
    screen_->display();
}

void screenDriver::reseting(){
    setup();
    screen_->println("Reseting network settings");
    screen_->display();
}

void screenDriver::connecting(bool mode){
    setup();
    if(mode){
    screen_->println("Connecting");
    }
    else{
    screen_->println("Failed to connect.");
    }
    screen_->display();
}