#include <DS3232RTC.h>
#include "Structure.h"
#include <Button.h>
#include "Alarm.h"
#include "Alarms.h"
#include "Display.h"
#include "ScreenManager.h"
#include "Screen.h"
#include "Buzzer.h"

// Pin configuration
const int latchPin = A2;  // ST_CP / rclk on shift register
const int clockPin = A1;  // SH_CP / srclk on shift register
const int dataPin = A3;   // DS / data on shift register
const int ldrPin = A0;    // LDR pin
const int buzzerPin = 13; // Buzzer pin
const int lButtonPin = 2; // Left button pin
const int mButtonPin = 3; // Middle button pin
const int rButtonPin = 4; // Right button pin

// Display
Display display(dataPin, clockPin, latchPin, ldrPin);

// Buzzer
Buzzer buzzer(buzzerPin);

// Buttons
Button lButton(lButtonPin);
Button mButton(mButtonPin);
Button rButton(rButtonPin);

// Screens
ScreenManager screenManager;

// Alarms
Alarms * alarms;

void setup() {
  // Init clock
  setSyncProvider(RTC.get);

  // Init buttons
  lButton.begin();
  mButton.begin();
  rButton.begin();

  // Init serial
  Serial.begin(9600);

  // Init alarms
  alarms = alarms->getInstance();
}

void loop() {
  // Check alarms
  if (alarms -> isActive()) {
    buzzer.playAlarm();
    if (mButton.pressed()) {
      alarms -> stop();
      buzzer.stopAlarm();
    }
  }

  // Get screen to process input and be displayed
  Screen *currentScreen = screenManager.getCurrentScreen();

  // Handle input
  if (lButton.pressed()) {
    currentScreen->leftButtonPressed();
  }
  if (rButton.pressed()) {
    currentScreen->rightButtonPressed();
  }
  if (mButton.pressed()) {
    currentScreen->middleButtonPressed();
  }

  // Update display
  display.display(currentScreen->getDisplay());

  //Check if there's anything on the serial port
  checkSerialUpdate();
}

void checkSerialUpdate() {
  // Check if new time has been pushed over serial (in format yy,m,d,h,m,s)
  if (Serial.available() != 0) {
    time_t t;
    tmElements_t tm;
    
    // Parse new datetime. Convert year from double digit to since 1970
    int y = Serial.parseInt();
    tm.Year = y2kYearToTm(y);
    tm.Month = Serial.parseInt();
    tm.Day = Serial.parseInt();
    tm.Hour = Serial.parseInt();
    tm.Minute = Serial.parseInt();
    tm.Second = Serial.parseInt();
    
    // Convert to time_t object
    t = makeTime(tm);
    
    // Set RTC and locally kept time
    RTC.set(t);
    setTime(t);

    //Clear any further serial data
    while (Serial.available() > 0) Serial.read();
  }
}
