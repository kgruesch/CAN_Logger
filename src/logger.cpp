#include <Arduino.h>
#include "logger.h"







int handleSerialCommands()
{
  static int vals[3] = {0};
  static int i = 0;

  while (Serial.available() > 0)
  {
    char c = Serial.read();

    switch (c)
    {
      case ' ':  // toggle command
        i = 0;   // reset state
        return -1;

      default:
        // convert ASCII -> hex nibble
        if      (c >= '0' && c <= '9') vals[i] = c - '0';
        else if (c >= 'A' && c <= 'F') vals[i] = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') vals[i] = c - 'a' + 10;
        else {
          // skip invalid chars quietly
          continue;
        }

        i++;

        // once 3 nibbles read, combine and return
        if (i == 3)
        {
          int ID = (vals[0] << 8) | (vals[1] << 4) | vals[2];
          Serial.print("Logging frame ID: 0x");
          Serial.println(ID, HEX);
          i = 0; // reset for next read
          return ID;
        }
        break;
    }
  }

  // no complete command yet
  return 0;
}
