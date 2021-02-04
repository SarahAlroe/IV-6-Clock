#include "Alarm.h"
#include "Arduino.h"

Alarm::Alarm(){
    enabled = false;
    weekdaysActive [7];
    fireHour = 7;
    fireMinute = 0;
}

void Alarm::enable() {
  enabled = true;
}

void Alarm::disable() {
  enabled = false;
}

void Alarm::toggleEnabled() {
  enabled = !enabled;
}

void Alarm::enableWeekday(int weekdayToEnable) {
  weekdaysActive[weekdayToEnable - 1] = true;
}

void Alarm::disableWeekday(int weekdayToDisable) {
  weekdaysActive[weekdayToDisable - 1] = false;
}

void Alarm::toggleWeekdayEnabled(int weekdayToToggle) {
  weekdaysActive[weekdayToToggle - 1] = !weekdaysActive[weekdayToToggle - 1];
}

void Alarm::setFireAt(int hourToFire, int minuteToFire) {
  setFireAtHour(hourToFire);
  setFireAtMinute(minuteToFire);
}

void Alarm::setFireAtHour(int hourToFire) {
  fireHour = hourToFire;
}

void Alarm::setFireAtMinute(int minuteToFire) {
  fireMinute = minuteToFire;
}

bool Alarm::isEnabled() {
  return enabled;
}

bool Alarm::firesOnWeekday(int testWeekday) {
  return weekdaysActive[(testWeekday - 1)];
}

int Alarm::hourToFire() {
  return fireHour;
}

int Alarm::minuteToFire() {
  return fireMinute;
}

bool Alarm::firesOn(int testWeekday, int testHour, int testMinute) {
  if (enabled) {
    if (weekdaysActive[testWeekday - 1]) {
      if (testHour == fireHour) {
        if (testMinute == fireMinute) {
          return true;
        }
      }
    }
  }
  return false;
}

void Alarm::setMelody(int melody){
  this->melody = melody;
}

int Alarm::getMelody(){
  return melody;
}
