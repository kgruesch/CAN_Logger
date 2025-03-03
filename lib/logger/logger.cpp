#include <Arduino.h>
#include "logger.h"







int loggerID(int valIn)
{   
    int ID;
    int i {0};
    int vals[3];
    int val = valIn;
    bool spacebar {false};
    bool logger {false};
    while (Serial.available() > 0 ) 
    { 
      val = (i==0)? valIn : Serial.read();
      
      if (i < 3 && !spacebar)
      {
        if (val < 60 && val > 47) //covers 0-9
        val -= 48;

        else if (val > 64 && val < 71) //covers A-F
        val -= 55;

        else if (val > 96 && val < 103)
        val -= 87;
        vals[i] = val;
      }
      i++;   
    }
      ID = (vals[0]<<8 | vals[1]<<4 | vals[2]);
      
      if (logger && ID != 0)
      {
        Serial.print("Logging frame ID: ");
        Serial.print("0x");
        Serial.print(ID, HEX);
        Serial.println();
        Serial.println("//////////////////////////////////////");
        Serial.println();
      }
      if (logger && ID == 0)
        Serial.println("Enter a frame ID to log");

      else if (!logger)
        Serial.println("logger stopped");

      
      spacebar = false;
      delay(500);
      return ID;
  }