#include <EEPROM.h>
#include <DS3232RTC.h>
#include <Button.h>
#include "Alarm.h"

//Fresh install overwrites things that may or may not matter for a first time setup
#define freshInstall false

//Display hex values
const byte dispNums[] = {0xB7, 0xA0, 0x3B, 0xB9, 0xAC, 0x9D, 0x8F, 0xB0, 0xBF, 0xBC}; //0-9 on display
const byte dotVal = 0x40; // Add this for dot on display
const byte emptyDisplay[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

int menuChoices [3] = {0, 0, 0};
int menuLevel = 0;
int mainMenuEntryCount = 5;
tmElements_t newTime;
const byte mainMenuTexts[5][6] = {{0xBF, 0xBE, 0x17, 0xAE, 0x00, 0x00}, {0xBE, 0x06, 0x8B, 0x0A, 0x8A, dispNums[1]}, {0xBE, 0x06, 0x8B, 0x0A, 0x8A, dispNums[2]}, {0xBE, 0x06, 0x8B, 0x0A, 0x8A, dispNums[3]}, {0x16, 0x02, 0x8A, 0x1F, 0x00, 0x00}}; //Back, Alarm1-3, Time
const byte timeSetTexts[6][6] = {{0xAD, 0x1F, 0x8B, 0x0A, 0x00, 0x00}, {0xB6, 0x8B, 0x8A, 0x0F, 0x00, 0x00}, {0xB7, 0x8B, 0xAD, 0x00, 0x00, 0x00}, {0xAE, 0x8B, 0x83, 0x0A, 0x00, 0x00}, {0xB6, 0x02, 0x8A, 0x00, 0x00, 0x00}, {0x9D, 0x1F, 0x17, 0x00, 0x00, 0x00}}; //Year00,Mont00,Day000,Hour00,Min000,Sec000
const byte alarmSetTexts[4][6] = {{0xBE, 0x00, 0x00, 0x00, 0x00, 0x00}, {0xBF, 0xBE, 0x17, 0xAE, 0x00, 0x00}, {0x16, 0x02, 0x8A, 0x1F, 0x00, 0x00}, {0xB7, 0x8B, 0xA7, 0x1F, 0x1F, 0xAE}}; //Ax0[0on/off], Back, Time, DoW
const byte weekDayTexts[8][6] = {{0x9D, 0x83, 0x8A, 0x00, 0x00, 0x00}, {0xB6, 0x8B, 0x8A, 0x00, 0x00, 0x00}, {0x16, 0x83, 0x1F, 0x00, 0x00, 0x00}, {0xA7, 0x1F, 0x8A, 0x00, 0x00, 0x00}, {0x16, 0x8E, 0x83, 0x00, 0x00, 0x00}, {0x16, 0x0A, 0x02, 0x00, 0x00, 0x00}, {0x9D, 0x8B, 0x0F, 0x00, 0x00, 0x00}, {0xBF, 0xBE, 0x17, 0xAE, 0x00, 0x00},}; //Sun,Mon.Tue,Wen,Thu,Fri,Sat,Sun, Back
int alarmMenuEntryCount = 4;
//This is not a state machine
enum appState {TIME, DATE, ALARMDISPLAY, MENU, DEMO};
appState currentState = TIME;

//Display control pins
const int latchPin = A2; //ST_CP / rclk on shift register
const int clockPin = A3; //SH_CP / srclk on shift register
const int dataPin = A1; //DS / data on shift register

//Buzzer pin
const int buzzerPin = 9;

//Buttons
Button lButton(2);
Button mButton(3);
Button rButton(4);

//Alarms
const int numAlarms = 3; //Number of alarms available
Alarm alarms [] = {Alarm(), Alarm(), Alarm()}; //Access this for alarming news
//Locations of alarm structs in eeprom, offset by randomly chosen value of 20
const int alarmAddress [] = {20, 21 + sizeof(alarms[0]), 22 + 2 * sizeof(alarms[0])};
bool alarmIsOn = false; //This var makes the beeps happen.
int alarmHourToSet;
int alarmMinuteToSet;

//Used for triggering things that should not happen constantly
int lastSec;
int lastMin;

long lastStateTime; //Used for switching states after a time

const long dateTimeOffset = 3; //How long in seconds to keep date showing
const long alarmShowTime = 2;

byte currentDisplayData[6]; //Whats on the display
byte targetDisplayData[6]; //What should be on the display. Used in future for animation

int displayBrightness = 10; //Val from 1-10.. More is more
int totalDelayTime = 10; //Higher gives more brightness resultion, but more flashy

const int analogBright = 600; //Analog value for a bright room. Experimentally chosen
const int analogDark = 100; //Analog value for a dark room. Experimentally chosen
int ambientBrightness = analogBright; //In which an averaged analog brightness value is stored

void setup() {
  //Init clock
  setSyncProvider(RTC.get);

  //Init buttons
  lButton.begin();
  mButton.begin();
  rButton.begin();

  //Init display control outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  //Init serial
  Serial.begin(9600);

  //Init alarms
  initAlarms();
}

void loop() {
  //Check if there's anything on the serial port
  checkSerialUpdate();
  //Handle buttons in all states but MENU
  if (currentState != MENU) {
    if (lButton.pressed()) {
      //Left button changes state to DATE
      currentState = DATE;
      lastStateTime = now();
    }
    if (rButton.pressed()) {
      //Right button switches state ALARMDISPLAY
      if (currentState == ALARMDISPLAY) {
        currentState = TIME;
      } else {
        currentState = ALARMDISPLAY;
      }
      lastStateTime = now();
    }
    if (mButton.pressed()) {
      //Middle button opens menu
      if (alarmIsOn) {
        alarmIsOn = false;
      } else {
        currentState = MENU;
      }
    }
  }

  switch (currentState) {
    //Choose what to display based on state
    case TIME:
      //In state TIME, screen is updated every second. Alarms are beeped if on
      if (lastSec != second()) {
        lastSec = second();
        displayTime();
        if (alarmIsOn) {
          tone(buzzerPin, 440, 500);
        }
      }
      break;
    case DATE:
      //In state DATE, date is displayed until time has passed, switches back to TIME
      displayDate();
      if (lastStateTime + dateTimeOffset < now()) {
        currentState = TIME;
      }
      break;
    case ALARMDISPLAY:
      //In state ALARMDISPLAY, alarms are displayed, passing through all three before switching back
      if (now() - lastStateTime > numAlarms * alarmShowTime) {
        currentState = TIME;
      }
      displayAlarm(min((now() - lastStateTime) / alarmShowTime, 2));
      break;
    case MENU:
      //Menu generation is big and bulky, handled elsewhere
      generateMenu();
      break;
    case DEMO:
      if (lastSec != second()) {
        lastSec = second();
        displayDemo();
      }
      break;
  }
  if (lastMin != minute()) {
    //Check alarms each minute
    lastMin = minute();
    checkAlarms();
  }
  //Ambient brightness is read to a moving average
  ambientBrightness = (ambientBrightness * 7 + analogRead(A0)) / 8;
  //Linearly map ambient brightness to display brightness
  displayBrightness = min(10, max(1, map(ambientBrightness, analogDark, analogBright, 1, 10)));
  //Push display, wait displaybrightness, push empty screen, wait rest of cycle. Practically software PWM...
  pushToDisplay(targetDisplayData);
  delay(displayBrightness);
  pushToDisplay(emptyDisplay);
  delay(totalDelayTime - displayBrightness);
}

void initAlarms() {
  //Save template alarms in eeprom if fresh install, else load them.
  if (freshInstall) {
    saveAlarms();
  } else {
    loadAlarms();
  }
}

void saveAlarms() {
  //Save alarms to eeprom if new
  for (int i = 0; i < numAlarms; i++) {
    EEPROM.put(alarmAddress[i], alarms[i]);
  }
}

void loadAlarms() {
  //Load alarms from EEPROM to memory
  for (int i = 0; i < numAlarms; i++) {
    EEPROM.get(alarmAddress[i], alarms[i]);
  }
}

void checkAlarms() {
  //Turn off alarm if it's been ringing for a minute
  alarmIsOn = false;
  //Check through all alarms and turn on if it's to fire this minute
  for (int i = 0; i < numAlarms; i++) {
    if (alarms[i].firesOn(weekday(), hour(), minute())) {
      alarmIsOn = true;
    }
  }
}

void generateMenu() {
  //This spaghetticode handles navigating the menu and pushing it to display.
  byte displayBytes[6];
  if (menuLevel == 0) {
    //In menuLevel 0 user can choose to set time or alarms, or go back to state TIME. l/r incs/decs curent menu choice. m proceeds deeper into menu.
    if (rButton.pressed()) {
      menuChoices[0] = modulo(menuChoices[0] + 1, mainMenuEntryCount);
    }
    if (lButton.pressed()) {
      menuChoices[0] = modulo(menuChoices[0] - 1, mainMenuEntryCount);
    }
    if (mButton.pressed()) {
      menuLevel = 1;
      menuChoices[1] = 0;
      menuChoices[2] = 0;
    }
    //Show currently chosen menu entry
    for (int i = 0; i < 6; i++) {
      displayBytes[i] = mainMenuTexts[menuChoices[0]][i];
    }
  } else if (menuLevel >= 1) {
    //When descending into menu, show new menu based on what was chosen previously
    if (menuChoices[0] == 0) {
      //Exit menu
      menuLevel = 0;
      currentState = TIME;
    } else if (menuChoices[0] == 4) {
      //Set time
      //L/R just incs decs a counter which is set/read to stored time on mclick
      if (lButton.pressed()) {
        menuChoices[2] -= 1;
      }
      if (rButton.pressed()) {
        menuChoices[2] += 1;
      }
      if (menuChoices[1] == 0) {
        //First set the adustable variable to currently stored year, proceed
        menuChoices[2] = year() - 2000;
        menuChoices[1] = 1;
      }
      if (menuChoices[1] == 1) {//Year
        //Make sure inced/deced value stays in this century
        menuChoices[2] = modulo(menuChoices[2], 100);
        //Calculate what to display
        for (int i = 0; i < 6; i++) {
          displayBytes[i] = timeSetTexts[menuChoices[1] - 1][i];
        }
        displayBytes[4] = dispNums[menuChoices[2] / 10];
        displayBytes[5] = dispNums[menuChoices[2] - (menuChoices[2] / 10) * 10];
        if (mButton.pressed()) {
          //Save new year to temp variable. Proceed to month
          newTime.Year = y2kYearToTm(menuChoices[2]);
          menuChoices[1] += 1;
          menuChoices[2] = month();
        }
      } else if (menuChoices[1] == 2) {//Month
        menuChoices[2] = modulo(menuChoices[2], 13);
        for (int i = 0; i < 6; i++) {
          displayBytes[i] = timeSetTexts[menuChoices[1] - 1][i];
        }
        displayBytes[4] = dispNums[menuChoices[2] / 10];
        displayBytes[5] = dispNums[menuChoices[2] - (menuChoices[2] / 10) * 10];
        if (mButton.pressed()) {
          //Store new month, proceed to day.
          newTime.Month = menuChoices[2];
          menuChoices[1] += 1;
          menuChoices[2] = day();
        }
      } else if (menuChoices[1] == 3) {//Day
        menuChoices[2] = modulo(menuChoices[2], 32);
        for (int i = 0; i < 6; i++) {
          displayBytes[i] = timeSetTexts[menuChoices[1] - 1][i];
        }
        displayBytes[4] = dispNums[menuChoices[2] / 10];
        displayBytes[5] = dispNums[menuChoices[2] - (menuChoices[2] / 10) * 10];
        if (mButton.pressed()) {
          //Store new day, proceed to hour.
          newTime.Day = menuChoices[2];
          menuChoices[1] += 1;
          menuChoices[2] = hour();
        }
      } else if (menuChoices[1] == 4) { //Hour
        menuChoices[2] = modulo(menuChoices[2], 24);
        for (int i = 0; i < 6; i++) {
          displayBytes[i] = timeSetTexts[menuChoices[1] - 1][i];
        }
        displayBytes[4] = dispNums[menuChoices[2] / 10];
        displayBytes[5] = dispNums[menuChoices[2] - (menuChoices[2] / 10) * 10];
        if (mButton.pressed()) {
          //Store new hour, proceed to minute.
          newTime.Hour = menuChoices[2];
          menuChoices[1] += 1;
          menuChoices[2] = minute();
        }
      } else if (menuChoices[1] == 5) { //Minute
        menuChoices[2] = modulo(menuChoices[2], 60);
        for (int i = 0; i < 6; i++) {
          displayBytes[i] = timeSetTexts[menuChoices[1] - 1][i];
        }
        displayBytes[4] = dispNums[menuChoices[2] / 10];
        displayBytes[5] = dispNums[menuChoices[2] - (menuChoices[2] / 10) * 10];
        if (mButton.pressed()) {
          //Store new minute, proceed to second.
          newTime.Minute = menuChoices[2];
          menuChoices[1] += 1;
          menuChoices[2] = second();
        }
      } else if (menuChoices[1] == 6) { //Second
        menuChoices[2] = modulo(menuChoices[2], 60);
        for (int i = 0; i < 6; i++) {
          displayBytes[i] = timeSetTexts[menuChoices[1] - 1][i];
        }
        displayBytes[4] = dispNums[menuChoices[2] / 10];
        displayBytes[5] = dispNums[menuChoices[2] - (menuChoices[2] / 10) * 10];
        if (mButton.pressed()) {
          //Store new second, proceed to saving new time in next cycle
          newTime.Second = menuChoices[2];
          menuChoices[1] += 1;
          menuChoices[2] = 0;
        }
      } else {
        //Make time_t object from new time values
        time_t t = makeTime(newTime);
        //Set time locally and on RTC
        RTC.set(t);
        setTime(t);
        //Exit back out to main menu
        menuLevel = 0;
      }
    } else {
      //If not time or exit, chosen menu entry must be one of the alarms
      if (menuLevel == 1) {
        //Some alarm menu entries have a deeper level, others dont. Are we still in upper alarm menu?
        if (rButton.pressed()) {
          menuChoices[1] = modulo(menuChoices[1] + 1, alarmMenuEntryCount);
        }
        if (lButton.pressed()) {
          menuChoices[1] = modulo(menuChoices[1] - 1, alarmMenuEntryCount);
        }

        for (int i = 0; i < 6; i++) {
          displayBytes[i] = alarmSetTexts[menuChoices[1]][i];
        }
        if (menuChoices[1] == 0) {
          //In menu option 0 alarm is either shown on or off. Clicking m button switches the alarm.
          displayBytes[1] = dispNums[menuChoices[0]];
          if (alarms[menuChoices[0] - 1].isEnabled()) {
            displayBytes[4] = 0xB7; //O
            displayBytes[5] = 0x8A; //n
          } else {
            displayBytes[3] = 0xB7; //O
            displayBytes[4] = 0x1E; //f
            displayBytes[5] = 0x1E; //f
          }
          if (mButton.pressed()) {
            alarms[menuChoices[0] - 1].toggleEnabled();
          }
        } else if (menuChoices[1] == 1) {
          //Menu option 1 allows exiting to main menu
          if (mButton.pressed()) {
            menuLevel = 0;
            //Calls saveAlarms before exiting
            saveAlarms();
          }
        } else if (menuChoices[1] == 2) {
          //Menu option 2 allows opening a submenu for setting fire time and minute
          if (mButton.pressed()) {
            menuLevel = 2;
          }
          //Read current settings
          alarmHourToSet = alarms[menuChoices[0] - 1].hourToFire();
          alarmMinuteToSet = alarms[menuChoices[0] - 1].minuteToFire();
        } else if (menuChoices[1] == 3) {
          //Menu option 3 allows opening a submenu for setting days of week active
          if (mButton.pressed()) {
            menuLevel = 2;
          }
        }
      } else { //Alarm submenu entered
        if (menuChoices[1] == 2) { //Adjust time
          if (mButton.pressed()) {
            menuChoices[2] += 1;
          }
          //Show hour and minute which alarm will be set to
          displayBytes[0] = dispNums[alarmHourToSet / 10];
          displayBytes[1] = dispNums[alarmHourToSet - (alarmHourToSet / 10) * 10];
          displayBytes[3] = dispNums[alarmMinuteToSet / 10];
          displayBytes[4] = dispNums[alarmMinuteToSet - (alarmMinuteToSet / 10) * 10];
          if (menuChoices[2] == 0) {
            //Adjust hours
            if (second() % 2 == 0) {
              //Blink hour dots to show active
              displayBytes[0] += dotVal;
              displayBytes[1] += dotVal;
            }
            //Inc/dec set hour
            if (rButton.pressed()) {
              alarmHourToSet = modulo(alarmHourToSet + 1, 24);
            }
            if (lButton.pressed()) {
              alarmHourToSet = modulo(alarmHourToSet - 1, 24);
            }
          } else if (menuChoices[2] == 1) {
            //Adjust minutes
            if (second() % 2 == 0) {
              //Blink minute dots to show active
              displayBytes[3] += dotVal;
              displayBytes[4] += dotVal;
            }
            if (rButton.pressed()) {
              alarmMinuteToSet = modulo(alarmMinuteToSet + 1, 60);
            }
            if (lButton.pressed()) {
              alarmMinuteToSet = modulo(alarmMinuteToSet - 1, 60);
            }
          } else {
            //Update alarm with new times and exit back to upper alarm menu
            alarms[menuChoices[0] - 1].setFireAt(alarmHourToSet, alarmMinuteToSet);
            menuLevel = 1;
          }
        } else { //Set days of week active
          if (rButton.pressed()) { //Inc/dec through days of week+back
            menuChoices[2] = modulo(menuChoices[2] + 1, 7 + 1);
          }
          if (lButton.pressed()) {
            menuChoices[2] = modulo(menuChoices[2] - 1, 7 + 1);
          }
          //Show currently selected weekday
          for (int i = 0; i < 6; i++) {
            displayBytes[i] = weekDayTexts[menuChoices[2]][i];
          }
          if (menuChoices[2] != 7) { //If on a weekday, show off or on
            if (alarms[menuChoices[0] - 1].firesOnWeekday(menuChoices[2] + 1)) {
              displayBytes[4] = 0xB7; //O
              displayBytes[5] = 0x8A; //n
            } else {
              displayBytes[3] = 0xB7; //O
              displayBytes[4] = 0x1E; //f
              displayBytes[5] = 0x1E; //f
            }

            if (mButton.pressed()) { //Switch day on/off
              alarms[menuChoices[0] - 1].toggleWeekdayEnabled(menuChoices[2] + 1);
            }
          } else {//If back button, exit to upper alarm menu level
            if (mButton.pressed()) {
              menuLevel = 1;
            }
          }
        }
      }
    }
  }
  //Show whatever menu data was prepared
  setTargetDisplayData(displayBytes);
}

void displayTime() {
  //Display RTC Time
  char displayElements[6];
  byte displayBytes[6];

  //Hour
  displayElements[0] = hour() / 10; //Get the tens - int division is always floored
  displayElements[1] = hour() - displayElements[0] * 10; //Get ones - just get rid of the tens

  //Minute
  displayElements[2] = minute() / 10;
  displayElements[3] = minute() - displayElements[2] * 10;

  //Second
  displayElements[4] = second() / 10;
  displayElements[5] = second() - displayElements[4] * 10;

  //Convert numbers to bytes for display
  for (int i = 0; i < 6; i++) {
    displayBytes[i] = dispNums[displayElements[i]];
  }

  //Every other second, blink the dots between hours, minutes and seconds
  if (displayElements[5] % 2 == 0) {
    //Only blink dots if alarm is on
    if (alarms[0].isEnabled()) {
      displayBytes[1] += dotVal;
    }
    if (alarms[1].isEnabled()) {
      displayBytes[3] += dotVal;
    }
    if (alarms[2].isEnabled()) {
      displayBytes[5] += dotVal;
    }
  }

  //Output the display data
  setTargetDisplayData(displayBytes);
}
void displayDate() {
  char displayElements[6];
  byte displayBytes[6];

  displayElements[0] = (year() - 2000) / 10; //Remove digits 3+4. Get the tens - int division is always floored
  displayElements[1] = (year() - 2000) - displayElements[0] * 10; //Get ones - just get rid of the tens

  //Minute
  displayElements[2] = month() / 10;
  displayElements[3] = month() - displayElements[2] * 10;

  //Second
  displayElements[4] = day() / 10;
  displayElements[5] = day() - displayElements[4] * 10;

  for (int i = 0; i < 6; i++) {
    displayBytes[i] = dispNums[displayElements[i]];
  }

  for (int i = 0; i < weekday() - 1; i++) {
    displayBytes[i] += dotVal;
  }
  setTargetDisplayData(displayBytes);
}
void displayAlarm(int alarmNumber) {
  char displayElements[6];
  byte displayBytes[6];

  //Hours
  displayElements[1] = alarms[alarmNumber].hourToFire() / 10;
  displayElements[2] = alarms[alarmNumber].hourToFire() - displayElements[1] * 10;

  //Minutes
  displayElements[3] = alarms[alarmNumber].minuteToFire() / 10;
  displayElements[4] = alarms[alarmNumber].minuteToFire() - displayElements[3] * 10;

  //Convert numbers to bytes for display
  for (int i = 0; i < 6; i++) {
    displayBytes[i] = dispNums[displayElements[i]];
  }


  //On / off
  if (alarms[alarmNumber].isEnabled()) {
    displayBytes[5] = 0xA0;
  } else {
    displayBytes[5] = 0x10;
  }
  //First and lastdisplay manually set
  displayBytes[0] = 0;

  //Add dots for days active. Sunday is a line.
  if (alarms[alarmNumber].firesOnWeekday(1)) {
    displayBytes[0] += 0x01;
  }
  for (int i = 0; i < 6; i++) {
    if (alarms[alarmNumber].firesOnWeekday(i + 2)) {
      displayBytes[i] += dotVal;
    }
  }
  //Light up segments on display 0 to show what alarm is displayed
  displayBytes[0] += 0x04;
  if (alarmNumber > 0) {
    displayBytes[0] += 0x10;
    if (alarmNumber > 1) {
      displayBytes[0] += 0x20;
    }
  }

  //Output
  setTargetDisplayData(displayBytes);

}
void displayDemo() {
  //Demo thingy, blinks screeen on/off every second
  byte displayBytes[6];

  if (second() % 2 == 0) {
    for (int i = 0; i < 6; i++) {
      displayBytes[i] = 0xFF;
      displayBrightness = random(1, 10);
    }
  } else {
    for (int i = 0; i < 6; i++) {
      displayBytes[i] = 0x00;
    }
  }
  setTargetDisplayData(displayBytes);
}

void setTargetDisplayData(byte displayData[]) {
  //Sets some new display data to change towards
  for (int i = 0; i < 6; i++) {
    targetDisplayData[i] = displayData[i];
  }
}

void pushToDisplay(byte displayData[]) {
  //Pushes 6 display bytes to the display
  //Pull latch low to prepare for new data
  digitalWrite(latchPin, LOW);
  for (int i = 0; i < 6; i++) {
    //Each byte for display is pushed inverted by ~, least significant byte first
    shiftOut(dataPin, clockPin, LSBFIRST, ~ displayData[5 - i]);
  }
  //Unlatch to show new data
  digitalWrite(latchPin, HIGH);
}

void checkSerialUpdate() {
  //Check if new time has been pushed over serial (in format yy,m,d,h,m,s)
  if (Serial.available() != 0) {
    time_t t;
    tmElements_t tm;
    //Parse new datetime. Convert year from double digit to since 1970
    int y = Serial.parseInt();
    tm.Year = y2kYearToTm(y);
    tm.Month = Serial.parseInt();
    tm.Day = Serial.parseInt();
    tm.Hour = Serial.parseInt();
    tm.Minute = Serial.parseInt();
    tm.Second = Serial.parseInt();
    //Convert to time_t object
    t = makeTime(tm);
    //Set RTC and locally kept time
    RTC.set(t);
    setTime(t);

    //Clear any further serial data
    while (Serial.available() > 0) Serial.read();
  }
}

int modulo(int dividend, int divisor) {
  while (dividend < 0) {
    dividend = dividend + divisor; //Wrap around to always get positive number
  }
  return dividend % divisor;
}

