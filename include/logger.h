#ifndef _LOGGER_H
#define _LOGGER_H

struct Logging{
    bool    logger  {false},
            garbage  {false},
            data  {false};
    int     ID {0},
            ID_all {false};
};

int handleSerialCommands();

#endif  