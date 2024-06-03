#include <../memorySocket/memorySocket.h>
#include <Audio.h>
#include <TinyGPS++.h>

#pragma once

struct Data{
    float lng;
    float lat;
};

class pendant
{
    private:
        Audio * audio_;

        String code_;
        Data data_;
        HardwareSerial * stream_;
    public:
        std::unique_ptr<memoryManager> mem;
        pendant(std::unique_ptr<memoryManager> mem, Audio* audio);

        bool Initialize();
        void SetCode(String& code);
        void PlayAudio(String message);
        String getCode();

        void loop();
};
