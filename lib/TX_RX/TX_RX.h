#ifndef TX_RX_H // 
#define TX_RX_H // 
#include <vector>

#define S1_UUID "ee5180e3-9dc1-436e-bdc7-463e0dbf36db"
#define C1_UUID "d0206f1d-ca15-46d2-8c32-3b2613eacccb" // app data UUID
#define C2_UUID "617abf97-dfe4-4cc8-aadc-464c7a5f33e1" // display data UUID

typedef struct struct_message {
    char a[32];
    int gear;
    int water;
    int oil;
    int rpm;
    int gearbox_temp;
    int illumination;
    int throttle;
    int modeData;
    int shift_Mode;
    bool pgmMode;
    int startFromSetMode;
} struct_message;

 typedef struct dataPacket {

    short gear;
    short water_temp;
    short oil_temp;
    short rpm;
    short gearbox_temp;
    short illumination;
    short throttle;
    short modeData;
    short shift_Mode;
    bool  pgmMode;
    bool  startFromSetMode;
    short oil_warning_temp; 
    short oil_danger_temp;   
    short shift;
    short shift_reminder;
    short RPMbar_temps;
    
} dataPacket;

std::vector<u_int8_t> writeAppData(std::vector<uint8_t>& appData);

std::vector<u_int8_t> initAppData(std::vector<uint8_t>& appData);
#endif
