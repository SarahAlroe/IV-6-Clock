#include  "Config.h""
#include <EEPROM.h>
#include <DS3232RTC.h>

Config *Config::instance = 0;

Config::Config() {
  this->load();
}

Config * Config::getInstance() {
  if (!instance)
    instance = new Config;
  return instance;
}

void Config::load() {
  EEPROM.get(SUMMER_TIME_ADDR, summerTime);
  EEPROM.get(TEMP_UNIT_ADDR, tempUnit);
  EEPROM.get(SNOOZE_TIME_ADDR, snoozeTime);
}

void Config::save() {
  EEPROM.put(SUMMER_TIME_ADDR, summerTime);
  EEPROM.put(TEMP_UNIT_ADDR, tempUnit);
  EEPROM.put(SNOOZE_TIME_ADDR, snoozeTime);
}

void Config::toggleSummerTime() {
  if (isSummerTime()) {
    setTime(now() - 3600);
    RTC.set(now());
  } else {
    setTime(now() + 3600);
    RTC.set(now());
  }
  summerTime = ! summerTime;
}

bool Config::isSummerTime() {
  return summerTime;
}

void Config::toggleTempUnit(){
  switch(tempUnit){
    case 'C':
      tempUnit = 'K';
      break;
    case 'K':
      tempUnit = 'F';
      break;
    case 'F':
      tempUnit = 'C';
      break;
  }
}

char Config::getTempUnit(){
  return tempUnit;
}

void Config::setSnoozeTime(int mins){
  snoozeTime = mins;
}

int Config::getSnoozeTime(){
  return snoozeTime;
}
