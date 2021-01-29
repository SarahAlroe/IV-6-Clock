#pragma once
#include <EEPROM.h>
#include "Alarm.h"

class Alarms {
  public:
    static Alarms *getInstance();
    Alarm * get(int i);
    void load();
    void save();
    bool isActive();
    void stop();
    static const int NUM_ALARMS = 3;
  private:
    static Alarms *instance;
    Alarms();
    Alarm alarms[NUM_ALARMS] = {Alarm(), Alarm(), Alarm()};
    const int alarmAddress [NUM_ALARMS] = {20, 21 + sizeof(alarms[0]), 22 + 2 * sizeof(alarms[0])};
    int lastMinute;
    bool alarmIsOn = false;
};
