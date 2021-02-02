#pragma once
#include "Structure.h"
#include "ScreenManager.h"
#include <DS3232RTC.h>

class Screen {
  public:
    virtual void init();
    virtual void leftButtonPressed();
    virtual void middleButtonPressed();
    virtual void rightButtonPressed();

    virtual struct DisplayData getDisplay();
};

class TimeScreen : public Screen {
  public:
    TimeScreen (ScreenManager *sm);

    void init();
    void leftButtonPressed();
    void middleButtonPressed();
    void rightButtonPressed();

    struct DisplayData getDisplay();

  private:
    ScreenManager *sm;
};

class DateScreen : public Screen {
  public:
    DateScreen (ScreenManager *sm);

    void init();
    void leftButtonPressed();
    void middleButtonPressed();
    void rightButtonPressed();

    struct DisplayData getDisplay();

  private:
    const long SCREEN_STAY_SECONDS = 3;

    ScreenManager *sm;
    long initTimestamp;
};

class AlarmScreen : public Screen {
  public:
    AlarmScreen (ScreenManager *sm);

    void init();
    void leftButtonPressed();
    void middleButtonPressed();
    void rightButtonPressed();

    struct DisplayData getDisplay();

  private:
    const long ALARM_SHOW_TIME = 4;

    ScreenManager *sm;
    long initTimestamp;
};

class TemperatureScreen : public Screen {
  public:
    TemperatureScreen (ScreenManager *sm);

    void init();
    void leftButtonPressed();
    void middleButtonPressed();
    void rightButtonPressed();

    struct DisplayData getDisplay();

  private:
    const long SCREEN_STAY_SECONDS = 4;
    const float TEMP_OFFSET = - 5.5 * 4.0; // Temperature offset to compensate for warm pcb. Multiply offset by 4 to get it to same scale as RTC output.

    ScreenManager *sm;
    long initTimestamp;
};


class MainMenuScreen : public Screen {
  public:
    MainMenuScreen (ScreenManager *sm);

    void init();
    void leftButtonPressed();
    void middleButtonPressed();
    void rightButtonPressed();

    struct DisplayData getDisplay();

  private:
    static const int BACK = 0, ALARM1 = 1, ALARM2 = 2, ALARM3 = 3, SET_TIME = 4, SUMMER_TIME = 5, TEMP_UNIT = 6, SNOOZE_TIME = 7;
    static const int MENU_ENTRY_COUNT = 8;
    const struct DisplayData menuText[MENU_ENTRY_COUNT] = {{{c['B'], c['a'], c['c'], c['k'], 0x00, 0x00}}, {{c['A'], c['l'], c['a'], c['r'], c['m'], c[1]}}, {{c['A'], c['l'], c['a'], c['r'], c['m'], c[2]}}, {{c['A'], c['l'], c['a'], c['r'], c['m'], c[3]}}, {{c['T'], c['i'], c['m'], c['e'], 0x00, 0x00}}, {{c['S'] + c['.'], c['T'] + c['.'], 0x00, 0x00, 0x00, 0x00}}, {{c['T'], c['e'], c['m'], c['p'], 0x00, 0x00}}, {{c['S'], c['n'], c['o'], c['o'], c['z'], c['e']}}};
    const long SCREEN_STAY_SECONDS = 30;

    ScreenManager *sm;
    int menuIndex = 0;
    long lastChangeTimestamp;
    bool inSubmenu = false;
};

class SetTimeScreen : public Screen {
  public:
    SetTimeScreen (ScreenManager *sm);

    void init();
    void leftButtonPressed();
    void middleButtonPressed();
    void rightButtonPressed();

    struct DisplayData getDisplay();

  private:
    static const int YEAR = 0, MONTH = 1, DAY = 2, HOUR = 3, MINUTE = 4, SECOND = 5;
    static const int MENU_ENTRY_COUNT = 6;
    const struct DisplayData menuText[MENU_ENTRY_COUNT] = {{{c['Y'], c['e'], c['a'], c['r'], 0x00, 0x00}}, {{c['M'], c['o'], c['n'], c['t'] , 0x00, 0x00}}, {{c['D'], c['a'], c['y'], 0x00, 0x00, 0x00}}, {{c['H'], c['o'], c['u'], c['r'], 0x00, 0x00}}, {{c['M'], c['i'], c['n'], 0x00, 0x00, 0x00}}, {{c['S'], c['e'], c['c'], 0x00, 0x00, 0x00}}};
    const long SCREEN_STAY_SECONDS = 30;

    ScreenManager *sm;
    int menuIndex = 0;
    int currentMenuValue = 0;
    tmElements_t newTime;
    long lastChangeTimestamp;

    int getUpperClamp();
    int getLowerClamp();
};

class SetAlarmScreen : public Screen {
  public:
    SetAlarmScreen (ScreenManager *sm, int alarmNumber);

    void init();
    void leftButtonPressed();
    void middleButtonPressed();
    void rightButtonPressed();

    struct DisplayData getDisplay();

  private:
    static const int ONOFF = 0, BACK = 1, TIME = 2, DOWEEK = 3;
    static const int MENU_ENTRY_COUNT = 4;
    const struct DisplayData menuText[MENU_ENTRY_COUNT] = {{{c['A'], 0x00, 0x00, 0x00, 0x00, 0x00}}, {{c['B'], c['a'], c['c'], c['k'], 0x00, 0x00}}, {{c['T'], c['i'], c['m'], c['e'], 0x00, 0x00}}, {{c['D'] | c['.'], c['o'] | c['.'], c['W'], c['e'], c['e'], c['k']}}};
    static const int W_BACK = 0, SUNDAY = 1, MONDAY = 2, TUESDAY = 3, WEDNESDAY = 4, THURSDAY = 5, FRIDAY = 6, SATURDAY = 7;
    static const int WEEKDAY_ENTRY_COUNT = 8;
    const struct DisplayData weekdayText[8] = { {{c['B'], c['a'], c['c'], c['k'], 0x00, 0x00}}, {{c['S'], c['u'], c['n'], 0x00, 0x00, 0x00}}, {{c['M'], c['o'], c['n'], 0x00, 0x00, 0x00}}, {{c['T'], c['u'], c['e'], 0x00, 0x00, 0x00}}, {{c['W'], c['e'], c['d'], 0x00, 0x00, 0x00}}, {{c['T'], c['h'], c['u'], 0x00, 0x00, 0x00}}, {{c['F'], c['r'], c['i'], 0x00, 0x00, 0x00}}, {{c['S'], c['a'], c['t'], 0x00, 0x00, 0x00}}};
    static const int HOUR = 0, MINUTE = 1;
    static const int TIME_ENTRY_COUNT = 2;
    const struct DisplayData timeText[8] = {{{c['H'], c['o'], c['u'], c['r'], 0x00, 0x00}}, {{c['M'], c['i'], c['n'], 0x00, 0x00, 0x00}}};
    const long SCREEN_STAY_SECONDS = 30;

    ScreenManager *sm;
    int alarmNumber = 0;
    int menuIndex = 0;
    bool inSubmenu = false;
    int submenuIndex = 0;
    long lastChangeTimestamp;
};
