#include "screenDriver.h"

screenDriver::screenDriver(TFT_eSPI * tft){tft_ = tft;}

void screenDriver::displayCode(String code){
    tft_->fillScreen(TFT_BLACK);
    tft_->drawString(code.c_str(),0,100);
}

void screenDriver::countdownSOS(int num){
    String text = "SOS mode in: " + num;
    tft_->drawString(text.c_str(),0,50);
}

void screenDriver::displaySOS(){
    tft_->drawString("SOS Mode ON", 0, 50);
    tft_->drawString("Click button to cancel", 0, 70);
}