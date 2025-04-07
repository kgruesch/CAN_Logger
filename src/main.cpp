#include <Arduino.h>
#include "driver/twai.h"
#include <vector>
#include "EEPROM.h"
#include "drive_mode.h"
#include "logger.h"
#include "TX_RX.h"
#include "SocketCAN.h"
#include <stdlib.h>
#define SDA 8
#define SCL 9

// put function declarations here:
int ID;
Logging logger;
int vREF = 5;
int MAP_pin = GPIO_NUM_4;
int IAT_pin = GPIO_NUM_5;
bool frameData = false;
bool header = false;
unsigned long timer1 = 0;
unsigned long timer2 = 0;

VehicleData CANdata;
Settings settings;
FrameLogging logFrames;

void setup() {
  neopixelWrite(21, OFF);
  Serial.begin(115200);
  neopixelWrite(21, YELLOW);
  adcAttachPin(MAP_pin);
  adcAttachPin(IAT_pin);
  delay(800);
  Serial.println("Serial on");
  Serial.print("log Rate set to: ");
  Serial.print(settings.logRate);
  Serial.println(" ms");
  CAN_start(2); //switch TX/RX/enable based on board version, see socketCAN.cpp
  Serial.println("CAN started");
  neopixelWrite(21, BLUE);
  delay(800);
}

void loop() 
{ 
  uint32_t MAP_volts = analogReadMilliVolts(MAP_pin); //input has 10k/10k voltage divider, range is 0.25 to 2.5 volts
  uint32_t IAT_volts = analogReadMilliVolts(IAT_pin);
  twai_message_t message;

  /// Type * to log all frames, or type a frame ID in hex (not case sensitive,format "0F3" or "0f3" for example) to log that frame only
  /// spacebar starts and stops the logger
  if (twai_receive(&message, pdMS_TO_TICKS(1)) == ESP_OK) {
    std::vector <int> CANframe(message.data_length_code);
      for (int i = 0; i < message.data_length_code; i++) {
        CANframe[i] = message.data[i];
      }
    CANUpdate(message.identifier, CANframe, CANdata);
      
    if (Serial.available() > 0 ) {
      int val = Serial.read();
      Serial.println("Console input received");

      if (val == 32) {//spacebar starts and stops the logger
        logger.logger = !logger.logger; //toggle logger on space bar entry
        logger.logger? Serial.println("Logger on") : Serial.println("Logger off");
        delay(500);
        logger.logger? neopixelWrite(21, GREEN) : neopixelWrite(21, OFF);
        while (Serial.available() > 0 ) //empty the buffer
        int trash = Serial.read();
      }
      else if (val == 42) { // type "*" to log all frames 
        logger.ID_all = true;
        Serial.print("Logging all frames");
        delay(500);
        settings.logRate < 20? settings.logRate = 20 : settings.logRate;
        while (Serial.available() > 0 ) //empty the buffer
          int trash = Serial.read();
      }
      else if (val == 35) { // type "#" to toggle frame logging 
        frameData = !frameData;
        logger.ID_all = false;
        header = false;
        while (Serial.available() > 0 ) //empty the buffer
        int trash = Serial.read();
      }
      else {
        logger.ID_all = false;
        logger.ID = loggerID(val, false); // Type a frame ID in hex to log that frame only
      
    }
    }
    if (logger.logger && (message.identifier == logger.ID || logger.ID_all) && !frameData) {
      if (millis() - timer1 > settings.logRate) {
        timer1 = millis();
      double time = millis() / 10;
      time = round(time);
      time = time/100;
      Serial.print(time);
      Serial.print("  ");
      Serial.print("ID: ");
      Serial.print("0x");
      Serial.print(message.identifier, HEX);
      Serial.print("\t");
      for(int i=0;i<8;i++) 
      { 
        Serial.print("  ");
        if (i >= message.data_length_code) {
          Serial.print("X");
        }
        else {
          Serial.print(message.data[i], DEC);
        }
        
      }
      Serial.print("  ");
  
      Serial.print(MAP_volts);
      Serial.print("  "); 

      Serial.print(IAT_volts);
      Serial.print("  ");

      Serial.print(CANdata.rpm);
      Serial.print("  ");
      Serial.print(CANdata.throttle);
      Serial.println();
      }
    }
    /////////////////////////////////////////////////////////////////////
    //////// Logging specific frames together //////////////////////////
    else if (frameData){
      int numFrames = logFrames.Frames.size();
      for (int i = 0; i < numFrames; i++) {
        if (logFrames.Frames[i] == message.identifier) {
          /// 16 bit frame
          if (logFrames.size[i] == 16) {
            int startByte = logFrames.startByte[i];
            uint16_t data = message.data[startByte + 1] << 8 | message.data[startByte];
            logFrames.data[i] = data;
            // apply 12 bit mask if applicable
            if (logFrames.mask[i]) {
              logFrames.data[i] = data & 0x0FFF;
            }
            
          }
          /// 8 bit frame
          else {
            logFrames.data[i] = message.data[logFrames.startByte[i]];
          }            
        }
      }
      if (millis() - timer2 > 93) {
        timer2 = millis();
        
        if (!header) {
          Serial.print("Time");
          Serial.print("  ");
          for (int i = 0; i < numFrames; i++) {
            Serial.print("0x");
            Serial.print(logFrames.Frames[i], HEX);
            Serial.print("-");
            Serial.print(logFrames.startByte[i]);
            Serial.print("  ");
          }
          Serial.print("MAP");
          Serial.print("  ");
          Serial.print("IAT");
          Serial.print("  ");
          Serial.print("RPM");
          Serial.print("  ");
          Serial.print("Thr");
          Serial.print("  ");
          Serial.println();
          header = true;
        }
        double timestamp = millis();
        Serial.print(timestamp/1000);
        Serial.print("  ");
        for (int i = 0; i < numFrames; i++) {
          Serial.print(logFrames.data[i], DEC);
          Serial.print("  ");
        }
        Serial.print("  ");
        Serial.print(MAP_volts);
        Serial.print("  ");
        Serial.print(IAT_volts);
        Serial.print("  ");
        Serial.print(CANdata.rpm);
        Serial.print("  ");
        Serial.print(CANdata.throttle);
        Serial.println(); 
      }
    }
  }
}

