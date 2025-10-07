#ifndef GLOBALS_H
#define GLOBALS_H

#pragma once
#include <Arduino.h>
#include <string>
#include "drive_mode.h"
#include "ui_manager.h"
#include "CAN_manager.h"
#include "globals.h"
#include "connection_manager.h"
#include "data_container.h"

extern VehicleData CANdata;
extern Settings settings;
extern Alerts alerts;
extern PreviousData prevData;
extern Flags flags;
extern Timers timers;
extern String jsonString;

#endif