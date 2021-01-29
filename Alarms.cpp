#include "Alarms.h"
#include <DS3232RTC.h>

Alarms *Alarms::instance = 0;

Alarms :: Alarms(){
  this->load();
}

Alarms * Alarms :: getInstance() {
  if (!instance)
    instance = new Alarms;
  return instance;
}

Alarm * Alarms :: get(int i){
  return &alarms[i];
}

void Alarms :: load(){
  //Load alarms from EEPROM to memory
  for (int i = 0; i < NUM_ALARMS; i++) {
    EEPROM.get(alarmAddress[i], alarms[i]);
  }
}

void Alarms :: save(){
  //Save alarms to eeprom if new
  for (int i = 0; i < NUM_ALARMS; i++) {
    EEPROM.put(alarmAddress[i], alarms[i]);
  }
}

bool Alarms :: isActive(){
  if (lastMinute != minute()){
    lastMinute = minute();
    for (int i = 0; i < NUM_ALARMS; i++) {
    if (alarms[i].firesOn(weekday(), hour(), minute())) {
      alarmIsOn = true;
    }
  }
  }
  return alarmIsOn;
}

void Alarms :: stop(){
  alarmIsOn = false;
}
