#pragma once
#include <EEPROM.h>
#include "Alarm.h"

class Alarms {
  public:
    static Alarms *getInstance();
    static const int NUM_ALARMS = 3;

    void load();
    void save();
    void stop();

    Alarm * get(int i);
    bool isActive();

  private:
    Alarms();
    static Alarms *instance;
    const int alarmAddress [NUM_ALARMS] = {20, 21 + sizeof(alarms[0]), 22 + 2 * sizeof(alarms[0])};

    Alarm alarms[NUM_ALARMS] = {Alarm(), Alarm(), Alarm()};
    int lastMinute;
    bool alarmIsOn = false;
};
