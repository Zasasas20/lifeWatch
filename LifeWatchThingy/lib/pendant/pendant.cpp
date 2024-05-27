#include "pendant.h"

pendant::pendant(std::unique_ptr<memoryManager> memo, Audio* audio, TinyGPSPlus * gps):
mem(std::move(memo)){
    audio_ = audio;
    gps_ = gps;
}

bool pendant::Initialize(){
    if(mem->isCodeSetup()){
        code_ = mem->getCode();
        return true;
    }
    else{
        return false;
    }
}

void pendant::SetCode(String& code){
    mem->setCode(code.c_str());
    delay(200);
}

void pendant::PlayAudio(String message){
    audio_->connecttospeech(message.c_str(), "en");
}

Data pendant::getGPSData(){
    while(Serial2.available() > 0){
      if (gps_->encode(Serial2.read())){
          if(gps_->location.isValid()){
            data_.lng = gps_->location.lng();
            data_.lat = gps_->location.lat();
            return data_;
          }
        }
    }
}

void pendant::loop(){
    audio_->loop();

}

String pendant::getCode(){
    return code_;
}
