#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#pragma once

class screenDriver{
    private:
        Adafruit_SSD1306 * screen_;

        void setup();
    public:
        screenDriver(Adafruit_SSD1306 * sc);

        void displayCode(String code);
        void displaySOS();
        void countdownSOS(int num);
        void reseting();
        void connecting(bool mode);
        void connected(String IP);
};   