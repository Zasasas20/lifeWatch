#include <TFT_eSPI.h>

#pragma once

class screenDriver{
    private:
        TFT_eSPI * tft_;

    public:
        screenDriver(TFT_eSPI * tft);

        void displayCode(String code);
        void displaySOS();
        void countdownSOS(int num);
};   