#include "screenDriver.h"

screenDriver::screenDriver(TFT_eSPI * tft){tft_ = tft;}

void screenDriver::displayCode(String code){
    tft_->fillScreen(TFT_BLACK);
    tft_->drawString(code.c_str(),0,100);
}