#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <../lib/webServer/APServer.h>
#include <../lib/pendant/pendant.h>
#include <../lib/lifewatch/lifewatch.h>

std::unique_ptr<memoryManager> mem = std::unique_ptr<memoryManager>(new memoryManager);
std::unique_ptr<APServer> server;
std::unique_ptr<lifewatch> LifeWatch;
TinyGPSPlus gps;
Adafruit_MPU6050 mpu;
Audio audio;

TFT_eSPI tft;

#define I2S_DOUT 12
#define I2S_BCLK 13
#define I2S_LRC 15
#define I2C_SDA 21
#define I2C_SCL 22

// DEBUG ONLY:
bool debugMode = true;
String SSID = "Zaid";
String Pass = "Holdonbro";

unsigned long lastMillis;

//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;  
unsigned long last_button_time = 0; 

void mqttWrapper(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total){
  LifeWatch->onMqttMessage(topic, payload, properties, len, index, total);
}

void setup() {

  Serial.begin(9600);

  Serial2.begin(9600,SERIAL_8N1,25,26);

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  audio.setVolume(17);

  Wire.begin(I2C_SDA, I2C_SCL);
  pinMode(I2C_SDA, INPUT_PULLUP);
  pinMode(I2C_SCL, INPUT_PULLUP);

  pinMode(2, INPUT_PULLUP);

  tft.init();

  tft.fillScreen(TFT_BLACK);
  tft.drawString("HIII", 0, 100);

  if (mem->isSetup() || debugMode){
    LifeWatch = std::unique_ptr<lifewatch>(new lifewatch(std::move(mem), &audio, IPAddress(80,115,229,72), debugMode, SSID, Pass, &tft));
    if (!mpu.begin()) {
      Serial.println("Failed to find MPU6050 chip");
    } else Serial.println("found MPU6050 chip");

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    LifeWatch->setCallback(mqttWrapper);
  }
  else{
    Serial.println("Read AP");
    server = std::unique_ptr<APServer>(new APServer(std::move(mem), &tft));
    server->connect();
  }

}

void loop() {
  if (server.get() != nullptr){
    server->loop();
  }
  else{
    while(Serial2.available() > 0){
      int state = digitalRead(2);
      button_time = millis();
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      // Get the magnitude of acceleration
      float total_accel = sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z);  

      if (state == LOW && button_time - last_button_time > 250){
        LifeWatch->flipMode();
        Serial.println("Pressed");
        last_button_time = button_time;
      } 
      else if (total_accel > 25){
        LifeWatch->fall();
      }

      if (gps.encode(Serial2.read())){
          if(gps.location.isValid()){
            if (millis() - lastMillis > 3000){
              LifeWatch->sendGPS(gps.location.lat(), gps.location.lng());
              lastMillis = millis();
            }
          }
        }
      LifeWatch->loop();
    }
  }
}
