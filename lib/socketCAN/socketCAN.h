#ifndef CAN_DATA_H // 
#define CAN_DATA_H // 

#include "drive_mode.h"

#define rpmID       0x0A5
#define temps       0x3F9
#define gearTemp    0x39A
#define gearID      0x0F3
#define lightID     0x202
#define modeID      0x3A7
#define parkID      0x197
#define throttleID  0x0D9
#define cruiseID    0x189
#define shiftMode   0x3FD
#define speedID     0x1A1
#define ethanolID   0x0EC

#define red     "#fc0505"
#define green   "#0cf54a"
#define blue    "#0539f7"
#define white   "#f7f8fa"
#define black   "#000000"

// void refresh_CAN_data(int identifier, struct Data);


struct VehicleData {
    int rpm {0};
    int gear {0};
    int water_temp {60};
    int oil_temp {60};
    int gearbox_temp {25};
    int illumination {0};
    int throttle {0};
    int modeData {0x19};
    int shift_Mode {0};
    int speed {0};
    int ethContent {0};
    int fuelTemp {0};
    bool crzResume {false};
    String gearString {"P"};
    String driveMode {"Comfort"};
};

struct Settings {
    int oil_warning_temp        {120};
    int oil_danger_temp         {130};
    int shiftLightRPM           {5500};
    int startType               {true};
    int startMode               {SPORT};
    int shift_reminder_ms       {5000};
};

struct Alerts {
    bool shiftReminder {false};
    bool oilWarning {false};
    bool oilDanger {false};
    bool coldEngine {false};
    bool shiftLight {false};
    bool lightsOn {false};
};

struct PreviousData {
    int     last_gear {0},
            lastDriveMode {19},  
            lastRPM {0};
};

void CAN_start(int version);

void CANUpdate(int frameID, std::vector<int>& frame, VehicleData& data);

void setAlerts(VehicleData& data, Alerts& alerts, Settings& settings);

#endif