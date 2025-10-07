#include <Arduino.h>
#include "logger.h"







int loggerID(int valIn, bool logRate)
{   
    int ID;
    int i {0};
    int vals[3];
    int val = valIn;
    while (Serial.available() > 0 && !logRate) 
    { 
      val = (i==0)? valIn : Serial.read();
      
      if (i < 3)
      {
        if (val < 60 && val > 47) //covers 0-9
        val -= 48;

        else if (val > 64 && val < 71) //covers A-F
        val -= 55;

        else if (val > 96 && val < 103) //covers a-f
        val -= 87;
        vals[i] = val;
      }
      i++;   
    }
      ID = (vals[0]<<8 | vals[1]<<4 | vals[2]);
      
      if (ID != 0)
      {
        Serial.print("Logging frame ID: ");
        Serial.print("0x");
        Serial.print(ID, HEX);
        Serial.println();
      }

      delay(500);
      return ID;

      while (Serial.available() > 0 && logRate) 
    { 
      val = (i==0)? valIn : Serial.read();
      
      if (i < 3)
      {
        if (val < 60 && val > 47) //covers 0-9
        val -= 48;

        vals[i] = val;
      }
      i++;   
    }
      ID = (vals[0]<<8 | vals[1]<<4 | vals[2]);

      delay(500);
      return ID;
  }