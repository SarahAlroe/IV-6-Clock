#include "Alarms.h"
#include <DS3232RTC.h>
#include "Config.h"

Alarms *Alarms::instance = 0;

Alarms::Alarms() {
  this->load();
}

Alarms * Alarms::getInstance() {
  if (!instance)
    instance = new Alarms;
  return instance;
}

Alarm * Alarms::get(int i) {
  return &alarms[i];
}

void Alarms::load() {
  // Load alarms from EEPROM to memory
  for (int i = 0; i < NUM_ALARMS; i++) {
    EEPROM.get(alarmAddress[i], alarms[i]);
  }
}

void Alarms::save() {
  // Save alarms to eeprom if new
  for (int i = 0; i < NUM_ALARMS; i++) {
    EEPROM.put(alarmAddress[i], alarms[i]);
  }
}

bool Alarms::isActive() {
  if (lastMinute != minute()) {
    lastMinute = minute();
    for (int i = 0; i < NUM_ALARMS; i++) {
      if (alarms[i].firesOn(weekday(), hour(), minute())) {
        alarmIsOn = true;
        activeAlarm = &alarms[i];
      }
    }
  }
  return alarmIsOn && snoozeUntil < now();
}

void Alarms::stop() {
  alarmIsOn = false;
}

void Alarms::snooze() {
  Config * config = config -> getInstance();
  snoozeUntil = now() + config -> getSnoozeTime() * 60; // SnoozeTime from minutes to seconds.
}

Alarm * Alarms::getActive() {
  return activeAlarm;
}
