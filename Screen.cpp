#include "Screen.h"
#include "Arduino.h"
#include <DS3232RTC.h>
#include "Alarm.h"
#include "Alarms.h"
#include "Config.h"

int modulo(int dividend, int lowerClamp, int divisor) {
  dividend -= lowerClamp;
  divisor -= lowerClamp;
  while (dividend < 0) {
    dividend = dividend + divisor; // Wrap around to always get positive number
  }
  return (dividend % divisor) + lowerClamp;
}

int modulo(int dividend, int divisor) {
  return modulo(dividend, 0, divisor);
}

// TimeScreen

TimeScreen::TimeScreen(ScreenManager *sm) {
  this->sm = sm;
}

void TimeScreen::init() {
}

void TimeScreen::leftButtonPressed() {
  sm->setCurrentScreen(sm->dateScreen);
}

void TimeScreen::middleButtonPressed() {
  sm->setCurrentScreen(sm->mainMenuScreen);
}

void TimeScreen::rightButtonPressed() {
  sm->setCurrentScreen(sm->alarmScreen);
}

struct DisplayData TimeScreen::getDisplay() {
  char displayElements[6];
  struct DisplayData displayData;

  // Hour
  displayElements[0] = hour() / 10; // Get the tens - int division is always floored
  displayElements[1] = hour() - displayElements[0] * 10; // Get ones - just get rid of the tens

  // Minute
  displayElements[2] = minute() / 10;
  displayElements[3] = minute() - displayElements[2] * 10;

  // Second
  displayElements[4] = second() / 10;
  displayElements[5] = second() - displayElements[4] * 10;

  // Convert numbers to bytes for display
  for (int i = 0; i < 6; i++) {
    displayData.d[i] = c[displayElements[i]];
  }

  // Every other second, blink the dots between hours, minutes and seconds if their alarm is on
  Alarms * alarms = alarms->getInstance();
  if (second() % 2 == 0) {
    if (alarms->get(0)->isEnabled()) {
      displayData.d[1] += c['.'];
    }
    if (alarms->get(1)->isEnabled()) {
      displayData.d[3] += c['.'];
    }
    if (alarms->get(2)->isEnabled()) {
      displayData.d[5] += c['.'];
    }
  }
  return displayData;
}

// DateScreen

DateScreen::DateScreen(ScreenManager *sm) {
  this->sm = sm;
}

void DateScreen::init() {
  initTimestamp = now();
}

void DateScreen::leftButtonPressed() {
  sm->setCurrentScreen(sm->temperatureScreen);
}

void DateScreen::middleButtonPressed() {
  sm->setCurrentScreen(sm->mainMenuScreen);
}

void DateScreen::rightButtonPressed() {
  sm->setCurrentScreen(sm->timeScreen);
}

struct DisplayData DateScreen::getDisplay()  {
  // Handle screen timeout
  if (initTimestamp + SCREEN_STAY_SECONDS < now()) {
    sm->setCurrentScreen(sm->timeScreen);
  }

  char displayElements[6];
  struct DisplayData displayData;

  // Year
  displayElements[0] = (year() - 2000) / 10; // Remove digits 3+4. Get the tens - int division is always floored
  displayElements[1] = (year() - 2000) - displayElements[0] * 10; // Get ones - just get rid of the tens

  // Month
  displayElements[2] = month() / 10;
  displayElements[3] = month() - displayElements[2] * 10;

  // Day
  displayElements[4] = day() / 10;
  displayElements[5] = day() - displayElements[4] * 10;

  // Convert numbers to bytes for display
  for (int i = 0; i < 6; i++) {
    displayData.d[i] = dispNums[displayElements[i]];
  }

  // Add a dot to the display for each day of the week passed. Sunday is 0 dots.
  for (int i = 0; i < weekday() - 1; i++) {
    displayData.d[i] += c['.'];
  }

  return displayData;
}

// AlarmScreen

AlarmScreen::AlarmScreen(ScreenManager *sm) {
  this->sm = sm;
}

void AlarmScreen::init() {
  initTimestamp = now();
}

void AlarmScreen::leftButtonPressed() {
  sm->setCurrentScreen(sm->timeScreen);
}

void AlarmScreen::middleButtonPressed() {
  sm->setCurrentScreen(sm->mainMenuScreen);
}

void AlarmScreen::rightButtonPressed() {
  sm->setCurrentScreen(sm->temperatureScreen);
}

struct DisplayData AlarmScreen::getDisplay()  {
  struct DisplayData displayData;
  Alarms * alarms = alarms->getInstance();

  // Handle screen timeout
  if (initTimestamp + ALARM_SHOW_TIME * alarms->NUM_ALARMS < now()) {
    sm->setCurrentScreen(sm->timeScreen);
  }

  // Dedicate equal amounts of the alarm screen show time to each alarm
  int alarmNumber = min((now() - initTimestamp) / ALARM_SHOW_TIME, 2);

  // Alarm number on first digit
  displayData.d[0] = c[alarmNumber + 1];

  Alarm * currentAlarm = alarms->get(alarmNumber);

  // On / off on second digit
  if (currentAlarm->isEnabled()) {
    displayData.d[1] = c['^'];
  } else {
    displayData.d[1] = c['-'];
  }

  // Hours on third and fourth digit
  displayData.d[2] = c[currentAlarm->hourToFire() / 10];
  displayData.d[3] = c[currentAlarm->hourToFire() % 10];

  // Minutes on fifth and sixth digit
  displayData.d[4] = c[currentAlarm->minuteToFire() / 10];
  displayData.d[5] = c[currentAlarm->minuteToFire() % 10];

  // Add dots for days active. Monday is a line on second digit
  if (currentAlarm->firesOnWeekday(1)) {
    displayData.d[0] += c['.'];
  }
  if (currentAlarm->firesOnWeekday(2)) {
    displayData.d[1] += c['_'];
  }
  for (int i = 3; i <= 7; i++) {
    if (currentAlarm->firesOnWeekday(i)) {
      displayData.d[i - 2] += c['.'];
    }
  }
  return displayData;
}

// TemperatureScreen

TemperatureScreen::TemperatureScreen(ScreenManager *sm) {
  this->sm = sm;
}

void TemperatureScreen::init() {
  initTimestamp = now();
}

void TemperatureScreen::leftButtonPressed() {
  sm->setCurrentScreen(sm->alarmScreen);
}

void TemperatureScreen::middleButtonPressed() {
  sm->setCurrentScreen(sm->mainMenuScreen);
}

void TemperatureScreen::rightButtonPressed() {
  sm->setCurrentScreen(sm->dateScreen);
}

int countDigits(int number) {
  int count = 0;
  while (number != 0) {
    number = number / 10;
    count++;
  }
  return count;
}

struct DisplayData TemperatureScreen::getDisplay()  {
  // Handle screen timeout
  if (initTimestamp + SCREEN_STAY_SECONDS < now()) {
    sm->setCurrentScreen(sm->timeScreen);
  }
  Config * config = config -> getInstance();

  struct DisplayData displayData = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
  float temp = RTC.temperature() + TEMP_OFFSET;
  switch (config->getTempUnit()) {
    case 'C':
      temp = temp / 4.0;
      displayData.d[4] = c['*'];
      displayData.d[5] = c['C'];
      break;
    case 'K':
      temp = temp / 4.0 + 273.15;
      displayData.d[5] = c['K'];
      break;
    case 'F':
      temp = temp * 9.0 / 5.0 + 32.0;
      displayData.d[4] = c['*'];
      displayData.d[5] = c['F'];
      break;
  }
  int tempInt = (int)(temp * 10.0);
  int tempDigits = countDigits(tempInt);
  for (int i = 0; i < tempDigits; i++) {
    displayData.d[i] = c[(tempInt / (int)pow(10, tempDigits - i - 1)) % 10];
  }
  displayData.d[tempDigits - 2] += c['.'];

  return displayData;
}

// MainMenuScreen

MainMenuScreen::MainMenuScreen(ScreenManager *sm) {
  this->sm = sm;
}

void MainMenuScreen::init() {
  lastChangeTimestamp = now();
}

void MainMenuScreen::leftButtonPressed() {
  lastChangeTimestamp = now();
  if (! inSubmenu) {
    menuIndex = modulo(menuIndex - 1, MENU_ENTRY_COUNT);
  } else if (menuIndex == SNOOZE_TIME) {
    Config * config = config -> getInstance();
    config->setSnoozeTime(max(config->getSnoozeTime()-1,1));
  }
}

void MainMenuScreen::rightButtonPressed() {
  lastChangeTimestamp = now();
  if (! inSubmenu) {
    menuIndex = modulo(menuIndex + 1, MENU_ENTRY_COUNT);
  } else if (menuIndex == SNOOZE_TIME) {
    Config * config = config -> getInstance();
    config->setSnoozeTime(min(config->getSnoozeTime()+1,99));
  }
}

void MainMenuScreen::middleButtonPressed() {
  Config * config = config -> getInstance();
  switch (menuIndex) {
    case BACK:
      config -> save();
      sm->setCurrentScreen(sm->timeScreen);
      break;
    case ALARM1:
      sm->setCurrentScreen(sm->setAlarmScreen1);
      break;
    case ALARM2:
      sm->setCurrentScreen(sm->setAlarmScreen2);
      break;
    case ALARM3:
      sm->setCurrentScreen(sm->setAlarmScreen3);
      break;
    case SET_TIME:
      sm->setCurrentScreen(sm->setTimeScreen);
      break;
    case SUMMER_TIME:
      config -> toggleSummerTime();
      break;
    case TEMP_UNIT:
      config -> toggleTempUnit();
      break;
    case SNOOZE_TIME:
      inSubmenu = ! inSubmenu;
  }
  lastChangeTimestamp = now();
}

struct DisplayData MainMenuScreen::getDisplay()  {
  // Handle screen timeout
  if (lastChangeTimestamp + SCREEN_STAY_SECONDS < now()) {
    sm->setCurrentScreen(sm->timeScreen);
  }
  Config * config = config -> getInstance();
  struct DisplayData displayData = menuText[menuIndex];
  if (menuIndex == SUMMER_TIME) {
    if (config->isSummerTime()) {
      displayData.d[4] = c['O'];
      displayData.d[5] = c['n'];
    } else {
      displayData.d[3] = c['O'];
      displayData.d[4] = c['f'];
      displayData.d[5] = c['f'];
    }
  } else if (menuIndex == TEMP_UNIT) {
    displayData.d[5] = c[config->getTempUnit()];
  } else if (menuIndex == SNOOZE_TIME && inSubmenu) {
    displayData.d[0] = c[config->getSnoozeTime() / 10];
    displayData.d[1] = c[config->getSnoozeTime() % 10];
    displayData.d[2] = 0;
    displayData.d[3] = c['m'];
    displayData.d[4] = c['i'];
    displayData.d[5] = c['n'];
  }
  return displayData;
}

// SetTimeScreen

SetTimeScreen::SetTimeScreen(ScreenManager *sm) {
  this->sm = sm;
}

void SetTimeScreen::init() {
  lastChangeTimestamp = now();
  menuIndex = 0;
  currentMenuValue = year() - 2000;
}

void SetTimeScreen::leftButtonPressed() {
  lastChangeTimestamp = now();
  currentMenuValue = modulo(currentMenuValue - 1, getLowerClamp(), getUpperClamp());
}

void SetTimeScreen::rightButtonPressed() {
  lastChangeTimestamp = now();
  currentMenuValue = modulo(currentMenuValue + 1, getLowerClamp(), getUpperClamp());
}

void SetTimeScreen::middleButtonPressed() {
  lastChangeTimestamp = now();
  // Cycle through each value to set, loading in the existing value each time.
  switch (menuIndex) {
    case YEAR:
      newTime.Year = y2kYearToTm(currentMenuValue);
      currentMenuValue = month();
      break;
    case MONTH:
      newTime.Month = currentMenuValue;
      currentMenuValue = day();
      break;
    case DAY:
      newTime.Day = currentMenuValue;
      currentMenuValue = hour();
      break;
    case HOUR:
      newTime.Hour = currentMenuValue;
      currentMenuValue = minute();
      break;
    case MINUTE:
      newTime.Minute = currentMenuValue;
      currentMenuValue = second();
      break;
    case SECOND:
      newTime.Second = currentMenuValue;

      // Make time_t object from new time values
      time_t t = makeTime(newTime);
      // Set time locally and on RTC
      RTC.set(t);
      setTime(t);

      sm->setCurrentScreen(sm->mainMenuScreen);
      break;
  }
  menuIndex ++;
}

struct DisplayData SetTimeScreen::getDisplay()  {
  // Handle screen timeout
  if (lastChangeTimestamp + SCREEN_STAY_SECONDS < now()) {
    sm->setCurrentScreen(sm->timeScreen);
  }
  struct DisplayData displayData = menuText[menuIndex];
  displayData.d[4] = c[currentMenuValue / 10];
  displayData.d[5] = c[currentMenuValue - ((currentMenuValue / 10) * 10)];
  return displayData;
}

int SetTimeScreen::getUpperClamp() {
  switch (menuIndex) {
    case YEAR:
      return 100;
    case MONTH:
      return 13;
    case DAY:
      return 32;
    case HOUR:
      return 24;
    case MINUTE:
      return 60;
    case SECOND:
      return 60;
  }
}

int SetTimeScreen::getLowerClamp() {
  switch (menuIndex) {
    case YEAR:
      return 0;
    case MONTH:
      return 1;
    case DAY:
      return 1;
    case HOUR:
      return 0;
    case MINUTE:
      return 0;
    case SECOND:
      return 0;
  }
}

// SetAlarmScreen

SetAlarmScreen::SetAlarmScreen(ScreenManager *sm, int alarmNumber) {
  this->sm = sm;
  this->alarmNumber = alarmNumber;
}
void SetAlarmScreen::init() {
  lastChangeTimestamp = now();
  menuIndex = 0;
}

void SetAlarmScreen::leftButtonPressed() {
  lastChangeTimestamp = now();
  if (! inSubmenu) {
    menuIndex = modulo(menuIndex - 1, MENU_ENTRY_COUNT);
  } else {
    if (menuIndex == DOWEEK) {
      submenuIndex = modulo(submenuIndex - 1, WEEKDAY_ENTRY_COUNT);
    } else if (menuIndex == TIME) {
      Alarms * alarms = alarms->getInstance();
      Alarm * currentAlarm = alarms->get(alarmNumber);
      if (submenuIndex == HOUR) {
        currentAlarm -> setFireAtHour(modulo(currentAlarm -> hourToFire() - 1, 24));
      } else if (submenuIndex == MINUTE) {
        currentAlarm -> setFireAtMinute(modulo(currentAlarm -> minuteToFire() - 1, 60));
      }
    }
  }
}

void SetAlarmScreen::rightButtonPressed() {
  lastChangeTimestamp = now();
  if (! inSubmenu) {
    menuIndex = modulo(menuIndex + 1, MENU_ENTRY_COUNT);
  } else {
    if (menuIndex == DOWEEK) {
      submenuIndex = modulo(submenuIndex + 1, WEEKDAY_ENTRY_COUNT);
    } else if (menuIndex == TIME) {
      Alarms * alarms = alarms->getInstance();
      Alarm * currentAlarm = alarms->get(alarmNumber);
      if (submenuIndex == HOUR) {
        currentAlarm -> setFireAtHour(modulo(currentAlarm -> hourToFire() + 1, 24));
      } else if (submenuIndex == MINUTE) {
        currentAlarm -> setFireAtMinute(modulo(currentAlarm -> minuteToFire() + 1, 60));
      }
    }
  }
}

void SetAlarmScreen::middleButtonPressed() {
  lastChangeTimestamp = now();
  Alarms * alarms = alarms->getInstance();
  Alarm * currentAlarm = alarms->get(alarmNumber);

  if (! inSubmenu) {
    switch (menuIndex) {
      case ONOFF:
        currentAlarm -> toggleEnabled();
        break;
      case BACK:
        alarms -> save();
        sm->setCurrentScreen(sm->mainMenuScreen);
        break;
      case TIME:
      case DOWEEK:
        submenuIndex = 0;
        inSubmenu = true;
        break;
    }
  } else {
    if (menuIndex == TIME) {
      if (submenuIndex == MINUTE) {
        inSubmenu = false;
      }
      submenuIndex ++;
    } else if (menuIndex == DOWEEK) {
      if (submenuIndex == W_BACK) {
        inSubmenu = false;
      } else {
        currentAlarm -> toggleWeekdayEnabled(submenuIndex);
      }
    }
  }
}

struct DisplayData SetAlarmScreen::getDisplay()  {
  // Handle screen timeout
  if (lastChangeTimestamp + SCREEN_STAY_SECONDS < now()) {
    sm->setCurrentScreen(sm->timeScreen);
  }
  Alarms * alarms = alarms->getInstance();
  Alarm * currentAlarm = alarms->get(alarmNumber);

  struct DisplayData displayData = menuText[menuIndex];

  if (menuIndex == ONOFF) {
    displayData.d[1] = c[alarmNumber + 1];
    if (currentAlarm->isEnabled()) {
      displayData.d[4] = c['O'];
      displayData.d[5] = c['n'];
    } else {
      displayData.d[3] = c['O'];
      displayData.d[4] = c['f'];
      displayData.d[5] = c['f'];
    }
  }

  else if (menuIndex == TIME && inSubmenu) {
    displayData = timeText[submenuIndex];
    int displayNumber;
    if (submenuIndex == HOUR) {
      displayNumber = currentAlarm -> hourToFire();
    } else {
      displayNumber = currentAlarm -> minuteToFire();
    }
    displayData.d[4] = c[displayNumber / 10];
    displayData.d[5] = c[displayNumber - ((displayNumber / 10) * 10)];
  }

  else if (menuIndex == DOWEEK && inSubmenu) {
    displayData = weekdayText[submenuIndex];
    if (submenuIndex != 0) {
      if (currentAlarm->firesOnWeekday(submenuIndex)) {
        displayData.d[4] = c['O'];
        displayData.d[5] = c['n'];
      } else {
        displayData.d[3] = c['O'];
        displayData.d[4] = c['f'];
        displayData.d[5] = c['f'];
      }
    }
  }

  return displayData;
}
