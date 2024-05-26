#include "pendant.h"

pendant::pendant(std::unique_ptr<memoryManager> memo, Audio* audio, TinyGPSPlus * gps, HardwareSerial *s):
mem(std::move(memo)){
    audio_ = audio;
    gps_ = gps;
    stream_ = s;
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
    if(stream_->available() > 0){
      if (gps_->encode(stream_->read())){
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
