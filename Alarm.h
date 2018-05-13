class Alarm {
  public:
    Alarm();

    void enable();
    void disable();
    void toggleEnabled();
    void enableWeekday(int weekdayToEnable);
    void disableWeekday(int weekdayToDisable);
    void toggleWeekdayEnabled(int weekdayToToggle);
    void setFireAt(int hourToFire, int minuteToFire);

    bool isEnabled();
    bool firesOnWeekday(int testWeekday);
    int hourToFire();
    int minuteToFire();

    bool firesOn(int testWeekday, int testHour, int testMinute);

  private:
    bool enabled;
    bool weekdaysActive [7];
    int fireHour;
    int fireMinute;

};

Alarm::Alarm() {
  enabled = false;
  bool weekdaysActive [7];
  fireHour = 7;
  fireMinute = 00;
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
  fireHour = hourToFire;
  fireMinute = minuteToFire;
}

bool Alarm::isEnabled() {
  return enabled;
}
bool Alarm::firesOnWeekday(int testWeekday) {
  Serial.println(weekdaysActive[testWeekday - 1]);
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

