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
