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

void setup() {
  neopixelWrite(21, OFF);
  Serial.begin(115200);
  neopixelWrite(21, YELLOW);
  delay(800);
  Serial.println("Serial on");
  CAN_start(2);
  Serial.println("CAN started");
  neopixelWrite(21, BLUE);
  delay(800);
}

void loop() 
{ 
  float MAP_volts = analogRead(4) * 2 / 4096 * vREF; //input has 10k/10k voltage divider, range is 0.25 to 2.5 volts
  float IAT_volts = analogRead(5) * 2 / 4096 * vREF;
  twai_message_t message;
  if (twai_receive(&message, pdMS_TO_TICKS(1)) == ESP_OK) {
      
  if (Serial.available() > 0 ) {
    int val = Serial.read();

    if (val == 32) {//spacebar starts and stops the logger
      logger.logger = !logger.logger; //toggle logger on space bar entry
      logger.logger? neopixelWrite(21, GREEN) : neopixelWrite(21, OFF);
      logger.ID_all = (!logger.logger)? false : logger.ID_all; //disables ID_all if logger is off
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
    
    double time = millis() / 10;
    time = round(time);
    time = time/100;
    Serial.print(time);
    Serial.print("\t");
    Serial.print("ID: ");
    Serial.print(message.identifier, HEX);
    Serial.print("\t");
    for(int i=0;i<message.data_length_code;i++) 
    {
      Serial.print(" ");
      if (message.data[i]<=10) 
      {
        Serial.print(0); //add a leading zero if the data is less than 10
      }
      Serial.print(message.data[i], DEC);
    }
    Serial.print("\t");
    Serial.print("MAP: ");
    Serial.print(MAP_volts);
    Serial.print("\t"); 
    Serial.print("IAT: ");
    Serial.print(IAT_volts);
    Serial.println();
  }
}
}
    


