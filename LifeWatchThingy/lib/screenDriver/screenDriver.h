#include <Arduino.h>

#pragma once

class screenDriver{
    private:

    public:
        screenDriver();

        void displayCode(String code);
        void displaySOS();
        void countdownSOS(int num);
};   