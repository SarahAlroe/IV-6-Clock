#pragma  once
#include <EEPROM.h>
#include "Alarm.h"

class Alarms {
  public:
    static Alarms *getInstance();
    static const int NUM_ALARMS = 3;

    void load();
    void save();
    void stop();
    void snooze();

    Alarm * get(int i);
    Alarm * getActive();
    bool isActive();

  private:
    Alarms();
    static Alarms *instance;
    Alarm alarms[NUM_ALARMS] = {Alarm(), Alarm(), Alarm()};
    const int alarmAddress [NUM_ALARMS] = {20, 21 + sizeof(alarms[0]), 22 + 2 * sizeof(alarms[0])};

    int lastMinute;
    bool alarmIsOn = false;
    unsigned long snoozeUntil = 0;
    Alarm * activeAlarm;
};
