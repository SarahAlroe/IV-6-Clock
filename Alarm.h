#pragma once

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
    void setFireAtHour(int hourToFire);
    void setFireAtMinute(int minuteToFire);
    void setMelody(int melody);

    bool isEnabled();
    bool firesOnWeekday(int testWeekday);
    int hourToFire();
    int minuteToFire();
    bool firesOn(int testWeekday, int testHour, int testMinute);
    int getMelody();

  private:
    bool enabled = false;
    bool weekdaysActive [7];
    int fireHour = 7;
    int fireMinute = 0;
    int melody = 1;
};
