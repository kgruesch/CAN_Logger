#include <Arduino.h>
#include "driver/twai.h"
#include <vector>
#include "EEPROM.h"
#include "drive_mode.h"
#include "logger.h"
#include "TX_RX.h"
#include "SocketCAN.h"
#include <stdlib.h>

// put function declarations here:
int ID;
Logging logger;
int vREF = 5;
int MAP_pin = GPIO_NUM_4;
int IAT_pin = GPIO_NUM_5;
unsigned long timer1 = 0;

VehicleData CANdata;

void setup() {
  neopixelWrite(21, OFF);
  Serial.begin(115200);
  neopixelWrite(21, YELLOW);
  adcAttachPin(MAP_pin);
  adcAttachPin(IAT_pin);
  delay(800);
  Serial.println("Serial on");
  CAN_start(2);
  Serial.println("CAN started");
  neopixelWrite(21, BLUE);
  delay(800);
}

void loop() 
{ 
  uint32_t MAP_volts = analogReadMilliVolts(MAP_pin); //input has 10k/10k voltage divider, range is 0.25 to 2.5 volts
  uint32_t IAT_volts = analogReadMilliVolts(IAT_pin);
  twai_message_t message;
  int interval = logger.ID_all? 50 : 5; // Slow down logging when pulling all frames, speed up for single frame logging
  if (twai_receive(&message, pdMS_TO_TICKS(1)) == ESP_OK) {
    std::vector <int> CANframe(message.data_length_code);
      for (int i = 0; i < message.data_length_code; i++) {
        CANframe[i] = message.data[i];
      }
    CANUpdate(message.identifier, CANframe, CANdata);
      
  if (Serial.available() > 0 ) {
    int val = Serial.read();

    if (val == 32) {//spacebar starts and stops the logger
      logger.logger = !logger.logger; //toggle logger on space bar entry
      logger.logger? neopixelWrite(21, GREEN) : neopixelWrite(21, OFF);
      while (Serial.available() > 0 ) //empty the buffer
      int trash = Serial.read();
    }
    else if (val == 42) { // type * to log all frames 
      logger.ID_all = true;
      while (Serial.available() > 0 ) //empty the buffer
        int trash = Serial.read();
    }
    else {
      logger.ID = loggerID(val); // Type a frame ID in hex to log that frame only
    }
  }
  if (logger.logger && (message.identifier == logger.ID | logger.ID_all)) {
    if (millis() - timer1 > interval) {
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
    Serial.print("MAP: ");
    Serial.print(MAP_volts);
    Serial.print("  "); 
    Serial.print("IAT: ");
    Serial.print(IAT_volts);
    Serial.print("  ");
    Serial.print("RPM: ");
    Serial.print(CANdata.rpm);
    Serial.print("  ");
    Serial.print("Thr: ");
    Serial.print(CANdata.throttle);
    Serial.println();
  }
}
}
}
    


