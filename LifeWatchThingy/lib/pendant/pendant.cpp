#include "pendant.h"

pendant::pendant(std::unique_ptr<memoryManager> memo, Audio* audio):
mem(std::move(memo)){
    audio_ = audio;
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

void pendant::loop(){
    audio_->loop();

}

String pendant::getCode(){
    return code_;
}
