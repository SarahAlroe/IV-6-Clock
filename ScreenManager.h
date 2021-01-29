#pragma once

class Screen;
class TimeScreen;
class DateScreen;
class AlarmScreen;
class MainMenuScreen;
class SetTimeScreen;
class SetAlarmScreen;

class ScreenManager {
  public:
    ScreenManager();
    ~ScreenManager();
    Screen * getCurrentScreen();
    void setCurrentScreen(Screen * newScreen);
    
    TimeScreen * timeScreen;
    DateScreen * dateScreen;
    AlarmScreen * alarmScreen;
    MainMenuScreen * mainMenuScreen;
    SetTimeScreen * setTimeScreen;
    SetAlarmScreen * setAlarmScreen1;
    SetAlarmScreen * setAlarmScreen2;
    SetAlarmScreen * setAlarmScreen3;
  private:
    Screen *currentScreen;
};
