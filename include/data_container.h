#ifndef DATA_CONTAINER_H
#define DATA_CONTAINER_H

#include <Arduino.h>
#include <vector>
#include <string>

struct VehicleData {
    int 
        rpm {0}, gear {0}, water_temp {60}, oil_temp {60}, gearbox_temp {25}, illumination{0}, throttle {0}, 
        modeData {0x19}, shift_Mode {0}, ethContent {0}, fuelTemp {0}, tachometer {0}, brake {0}, ambient_mBar;
    float 
        speed {0}, accel_y {0}, accel_x {0}, gyro {0}, boost {0}, IAT_volts {0}, Oil_pressure {0}, aux_volts {0};
    bool  
        brakeOn {false}, crzResume {false}, ToggleUp {false}, ToggleUpUp {false}, ToggleDown {false},
        crzOn {false}, crzSet {false};
    String 
        gearString {"P"}, driveMode {"Comfort"};
};

struct Settings {
    int 
        oil_warning_temp {108}, coolant_warning_temp  {108}, shiftLightRPM {5500}, startType {true}, startMode {SPORT},
        shift_reminder_ms {5000}, sendRate {50}, DSC_display_time_ms {5000}, logRate {20}, tireType {0};
};



struct Alerts {
    bool 
        shiftReminder {false}, oilWarning {false}, coolWarning {false}, coldEngine {false}, shiftLight {false}, lightsOn {false};
};

struct PreviousData {
    int     
        last_gear {0}, lastDriveMode {19}, brake {0}, lastRPM {0};
    bool lastUpStatus {false}, lastDownStatus {false};
    unsigned long DSC_timer {0};
};

struct Timers {
    unsigned long    //timers
        rpmTimer {0}, DSC_timer {0}, ECOTimer {0}, wait {0}, monitor {0}, longPressTimer {0}, DSC_displayTimer {0};
};

struct Flags {
    bool 
        startUpComplete {false}, pressed {false}, DSC_switch {false}, released {false};
};
#endif

