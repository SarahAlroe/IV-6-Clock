#include "Screen.h"

ScreenManager :: ScreenManager(){
  timeScreen = new TimeScreen(this);
  dateScreen = new DateScreen(this);
  alarmScreen = new AlarmScreen(this);
  mainMenuScreen = new MainMenuScreen(this);
  setTimeScreen = new SetTimeScreen(this);
  setAlarmScreen1 = new SetAlarmScreen(this,0);
  setAlarmScreen2 = new SetAlarmScreen(this,1);
  setAlarmScreen3 = new SetAlarmScreen(this,2);
  setCurrentScreen(timeScreen);
}

ScreenManager :: ~ScreenManager(){
  delete timeScreen;
}

Screen * ScreenManager::getCurrentScreen(){
  return currentScreen;
}

void ScreenManager::setCurrentScreen(Screen * newScreen){
  currentScreen = newScreen;
  currentScreen -> init();
}
